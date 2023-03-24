// vmem.c
#include <vmem.h>
#include <utilities/string.h>
#include <signal.h>
#include <asm/spriv.h>
#include <console.h>
#include <kimage_defs.h>
#include <device/memory.h>

pagetable_t kernel_pagetable;

pte_t *vm_walk(pagetable_t pgtbl, uint64 va, int alloc)
{
    for (int lv = VA_N_VPN - 1; lv; lv--)
    {
        pte_t *e = &pgtbl[VA_GET_PN(lv, va)];
        if ((*e) & PTE_FLAG_V)
            pgtbl = (pagetable_t)(PTE_PA(*e));
        else
        {
            if (!alloc || (pgtbl = (pagetable_t)kmem_alloc()) == 0)
                return NULL;
            memset(pgtbl, 0, PAGE_SIZE);
            *e = PA_PTE(PMA_VA2PA(pgtbl)) | PTE_FLAG_V;
        }
    }
    return &pgtbl[VA_GET_PN(0, va)];
}

// pgtbl points to the virtual address of the pagetable;
// leaves must be cleared;
void vm_freewalk(pagetable_t pgtbl)
{
    for (int i = 0; i < (1 << 9); i++)
    {
        pte_t pte = pgtbl[i];
        if ((pte & PTE_FLAG_V) && (pte & (PTE_FLAG_R | PTE_FLAG_W | PTE_FLAG_X)) == 0)
        {
            // non-leaf;
            uint64 nxt = PMA_PA2VA(PTE_PA(pte));
            vm_freewalk((pagetable_t)nxt);
            pgtbl[i] = 0;
        }
        else if (pte & PTE_FLAG_V)
            panic("vm_freewalk - leaves");
    }
    kmem_free(pgtbl);
}

int vm_mappages(pagetable_t pgtbl, uint64 va, uint64 pa, size_t siz, uint64 flags)
{
    if (siz == 0)
        return -1;
    uint64 current_pg = PAGE_ROUND_DOWN(va), end = PAGE_ROUND_DOWN(va + siz - 1);
    for (;;)
    {
        pte_t *entry = vm_walk(pgtbl, current_pg, 1);
        if (entry == NULL)
            return -1;
        if ((*entry) & PTE_FLAG_V)
            panic("vm_mappages: overlapped");
        *entry = PA_PTE(pa) | PTE_FLAG_V | flags;
        // unsigned int cannot be used in for loop condition;
        // casuing problem when reaching the edge of the virtual memory;
        if (current_pg == end)
            break;
        current_pg += PAGE_SIZE, pa += PAGE_SIZE;
    }
    return 0;
}

void vm_hart_enable(void)
{
    sfence_vma();
    w_satp(SATP(VMEM_MODE, PMA_VA2PA(kernel_pagetable)));
    sfence_vma();
}

pagetable_t vm_kernel_make_pagetable(void)
{
    pagetable_t tbl = (pagetable_t)kmem_alloc();
    // Device registers;
    vm_mappages(tbl, PMA_PA2VA(0x0), 0x0, 0x80000000, PTE_FLAG_R | PTE_FLAG_W);
    // Kernel text;
    vm_mappages(tbl, PMA_PA2VA(KERNEL_TEXT_PA_BEGIN), (uint64)KERNEL_TEXT_PA_BEGIN, (uint64)KERNEL_RODATA_PA_BEGIN - (uint64)KERNEL_TEXT_PA_BEGIN, PTE_FLAG_R | PTE_FLAG_X);
    // Kernel data;
    vm_mappages(tbl, PMA_PA2VA(KERNEL_RODATA_PA_BEGIN), (uint64)KERNEL_RODATA_PA_BEGIN, (uint64)KERNEL_IMG_PA_END - (uint64)KERNEL_RODATA_PA_BEGIN, PTE_FLAG_R | PTE_FLAG_W);
    // Kernel memory pool;
    for (int seg_id = 0; seg_id < ram_descriptor.available_ram_segments_num; seg_id++)
    {
        uint64 pa = ram_descriptor.available_ram_segments[seg_id].pa_begin;
        size_t len = ram_descriptor.available_ram_segments[seg_id].pa_len;
        vm_mappages(tbl, PMA_PA2VA(pa), (uint64)pa, len, PTE_FLAG_R | PTE_FLAG_W);
    }
    return tbl;
}

void vm_kernel_init(void)
{
    kernel_pagetable = vm_kernel_make_pagetable();
    printf("[vm]vkernel pagetable mapped\n");
}

pagetable_t vm_user_make_pagetable(void)
{
    pagetable_t ret = kmem_alloc();
    if (ret == NULL)
        panic("vm_user_make_pagetable - no space");
    // TODO;
}

uint64 vm_user_walk_addr(pagetable_t pgtbl, uint64 va)
{
    pte_t *pte = vm_walk(pgtbl, va, 0);
    if (pte == NULL || (*pte & PTE_FLAG_V) == 0 || (*pte & PTE_FLAG_U) == 0)
        return NULL;
    return PTE_PA(pte) | (va & 0x1FF);
}

// va must be aligned;
void vm_unmappages(pagetable_t pgtbl, uint64 va, size_t page_count, int do_free)
{
    if (va & 0x1FF)
        panic("vm_unmappages - broken alignment");
    uint64 bound = va + PAGE_SIZE * page_count;
    for (uint64 v_addr = va; v_addr < bound; v_addr += PAGE_SIZE)
    {
        pte_t *entry = vm_walk(pgtbl, v_addr, 0);
        if (entry == NULL || (*entry & PTE_FLAG_V) == 0 || (*entry & ((1ull << PTE_FLAGS_WIDTH) - 1)) == PTE_FLAG_V)
            panic("vm_unmappages - broken entry");
        if (do_free)
            kmem_free((void *)PMA_PA2VA(PTE_PA(*entry)));
        *entry = 0;
    }
}