// vmem.c
#include <vmem.h>
#include <utilities/string.h>
#include <utilities/bytes.h>
#include <signal.h>
#include <asm/spriv.h>
#include <asm/csr.h>
#include <console.h>
#include <kimage_defs.h>
#include <device/memory.h>

pagetable_t kernel_pagetable;
static pte_t *pkernel_pgtbl[VA_N_VPN - 2];

extern char user_handler[];

pte_t *vm_walk(pagetable_t pgtbl, ulong va, int alloc)
{
    for (int lv = VA_N_VPN - 1; lv; lv--)
    {
        pte_t *e = &pgtbl[VA_GET_PN(lv, va)];
        if ((*e) & PTE_FLAG_V)
            pgtbl = (pagetable_t)PMA_PA2VA(PTE_PA(*e));
        else
        {
            if (!alloc || (pgtbl = (pagetable_t)kmem_alloc_pages(0)) == 0)
                return NULL;
            memset(pgtbl, 0, PAGE_SIZE);
            *e = (PA_PTE(PMA_VA2PA(pgtbl)) | PTE_FLAG_V);
        }
    }
    return &pgtbl[VA_GET_PN(0, va)];
}

int vm_mappages(pagetable_t pgtbl, ulong va, ulong pa, size_t siz, ulong flags)
{
    if (siz == 0)
        return -1;
    for (uint64 current_pg = PAGE_ROUND_DOWN(va);
         current_pg != PAGE_ROUND_UP(va + siz);
         current_pg += PAGE_SIZE, pa += PAGE_SIZE)
    {
        pte_t *entry = vm_walk(pgtbl, current_pg, 1);
        if (entry == NULL)
            return -1;
        if ((*entry) & PTE_FLAG_V)
            panic("vm_mappages - overlapped");
        *entry = (PA_PTE(pa) | PTE_FLAG_V | flags);
    }
    return 0;
}

void vm_unmappages(pagetable_t pgtbl, ulong va, size_t page_count, int do_free)
{
    if (va & ((1ul << PN_WIDTH) - 1))
        panic("vm_unmappages - broken alignment");
    uint64 bound = va + PAGE_SIZE * page_count;
    for (uint64 v_addr = va; v_addr < bound; v_addr += PAGE_SIZE)
    {
        pte_t *entry = vm_walk(pgtbl, v_addr, 0);
        if (entry == NULL || ((*entry) & PTE_FLAG_V) == 0 || (*entry & ((1ul << PTE_FLAGS_WIDTH) - 1)) == PTE_FLAG_V)
            panic("vm_unmappages - broken entry");
        if (do_free)
            kmem_free_page((void *)PMA_PA2VA(PTE_PA(*entry)));
        *entry = 0;
    }
}

void vm_reap_pagetable(pagetable_t pgtbl)
{
    for (int i = 0; i < (1 << PN_WIDTH); i++)
    {
        pte_t pte = pgtbl[i];
        if ((pte & PTE_FLAG_V) && (pte & (PTE_FLAG_R | PTE_FLAG_W | PTE_FLAG_X)) == 0)
        {
            // non-leaf;
            uint64 nxt = PMA_PA2VA(PTE_PA(pte));
            vm_reap_pagetable((pagetable_t)nxt);
            pgtbl[i] = 0;
        }
        else if (pte & PTE_FLAG_V)
            // no binding pages are allowed;
            panic("vm_reap_pagetable - leaves");
    }
    kmem_free_page(pgtbl);
}

void vm_reap_pagetable_force(pagetable_t pgtbl)
{
    for (int i = 0; i < (1 << PN_WIDTH); i++)
    {
        pte_t pte = pgtbl[i];
        if ((pte & PTE_FLAG_V) && (pte & (PTE_FLAG_R | PTE_FLAG_W | PTE_FLAG_X)) == 0)
        {
            // non-leaf;
            uint64 nxt = PMA_PA2VA(PTE_PA(pte));
            vm_reap_pagetable_force((pagetable_t)nxt);
            pgtbl[i] = 0;
        }
    }
    kmem_free_page(pgtbl);
}

void vm_map_user_handler(pagetable_t pgtbl) { vm_mappages(pgtbl, VA_USER_USER_HANDLER_BEGIN, (ulong)KERNEL_USER_HANDLER_PA_BEGIN, VA_USER_USER_HANDLER_SIZE, PTE_FLAG_R | PTE_FLAG_X); }

void vm_unmap_user_handler(pagetable_t pgtbl) { vm_unmappages(pgtbl, VA_USER_USER_HANDLER_BEGIN, VA_USER_USER_HANDLER_SIZE >> PAGE_SHIFT, false); }

void vm_kernel_init(void)
{
    kernel_pagetable = (pagetable_t)PMA_PA2VA((r_satp() & ((1ul << VMEM_MODE_SHIFT) - 1)) << PAGE_SHIFT);
    ulong kpgtbl_base_addr = (ulong)kernel_pagetable - (VA_N_VPN - 3) * (1 << PN_WIDTH);
    for (int i = 0; i < VA_N_VPN - 2; i++, kpgtbl_base_addr += (1 << PN_WIDTH))
        pkernel_pgtbl[i] = (pte_t *)kpgtbl_base_addr;
    printf("[vm]kernel pagetable binded in vkernel\n");
}

void vm_kernel_remove_idmap(void)
{
    // remove the identical mapping at user address space;
    for (ulong phy_pn = 0; phy_pn != (1ul << (PN_WIDTH - 1)); phy_pn++)
        pkernel_pgtbl[VA_N_VPN - 3][phy_pn] = 0;
    printf("[vm]temporary user-space mapping removed\n");
    // mapping the trapframe;
    vm_map_user_handler(kernel_pagetable);
    printf("[vm]mapped the trapframe\n");
}

void vm_hart_enable(void)
{
    sfence_vma();
    w_satp(SATP(VMEM_MODE, PMA_VA2PA(pkernel_pgtbl[VA_N_VPN - 3])));
    sfence_vma();
}

pagetable_t vm_user_make_pagetable(void)
{
    pagetable_t ret = kmem_alloc_pages(0);
    memset(ret, 0, PAGE_SIZE);
    if (ret == NULL)
        panic("vm_user_make_pagetable - no space");
    vm_map_user_handler(ret);
    return ret;
}

void *vm_translate(pagetable_t pgtbl, void *addr)
{
    pte_t *entry = vm_walk(pgtbl, (ulong)addr, 0);
    if (entry == NULL)
        return NULL;
    return (void *)PMA_PA2VA(PTE_PA(*entry) | ((ulong)addr & (PAGE_SIZE - 1)));
}

static void vm_uvmcpy_recursive(pagetable_t pgtbl, pagetable_t new_pgtbl, int level, int overwite)
{
    for (ulong pn = 0; pn != (1 << PN_WIDTH); pn++)
    {
        pte_t pte = pgtbl[pn];
        if (pte & PTE_FLAG_V)
        {
            if (pte & (PTE_FLAG_R | PTE_FLAG_X))
            {
                if (!overwite && new_pgtbl[pn] != 0)
                    continue;
                // leaf entry;
                // copy the content inside and re-pointing at the new page;
                ulong new_page = (ulong)kmem_alloc_pages(0);
                memcpy((void *)new_page, (const void *)PMA_PA2VA(PTE_PA(pte)), PAGE_SIZE);
                // make flags and pa together;
                new_pgtbl[pn] = (PA_PTE(PMA_VA2PA(new_page)) | (pte & ((1ul << PTE_FLAGS_WIDTH) - 1)));
            }
            else if (level > 0)
            {
                if (new_pgtbl[pn] == 0)
                {
                    // make a new sub pagetable;
                    pagetable_t new_subpgtble = (pagetable_t)kmem_alloc_pages(0);
                    new_pgtbl[pn] = (PA_PTE(PMA_VA2PA(new_subpgtble)) | (pte & ((1ul << PTE_FLAGS_WIDTH) - 1)));
                }
                vm_uvmcpy_recursive((pagetable_t)PMA_PA2VA(PTE_PA(pgtbl[pn])), (pagetable_t)PMA_PA2VA(PTE_PA(new_pgtbl[pn])), level - 1, overwite);
            }
        }
    }
}

void vm_uvmcpy(pagetable_t pgtbl, pagetable_t new_pgtbl, int overwite) { vm_uvmcpy_recursive(pgtbl, new_pgtbl, VA_N_VPN - 1, overwite); }

int vm_memcpyin(pagetable_t pgtbl, void *dst, void *vsrc, size_t size)
{
    void *dst_end = dst + size;
    while (dst != dst_end)
    {
        char *tmp_ptr = (char *)vm_translate(pgtbl, vsrc);
        if (tmp_ptr != NULL)
            *((char *)dst) = *tmp_ptr;
        else
            return -1;
        dst++, vsrc++;
    }
    return 0;
}

int vm_memcpyout(pagetable_t pgtbl, void *vdst, void *src, size_t size)
{
    void *vdst_end = vdst + size;
    while (vdst != vdst_end)
    {
        char *tmp_ptr = (char *)vm_translate(pgtbl, vdst);
        if (tmp_ptr != NULL)
            *tmp_ptr = *((char *)src);
        else
            return -1;
        vdst++, src++;
    }
    return 0;
}
