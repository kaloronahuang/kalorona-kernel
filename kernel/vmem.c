// vmem.c
#include <vmem.h>
#include <string.h>
#include <signal.h>
#include <asm/spriv.h>
#include <console.h>

pagetable_t kernel_pagetable;

extern char KERNEL_TEXT_BEGIN[];
extern char KERNEL_TEXT_END[];
extern char KERNEL_END[];

pte_t *vm_walk(pagetable_t pgtbl, uint64 va, int alloc)
{
    if (va >= MAX_VA)
        return NULL;
    for (int lv = VMEM_LEVEL; lv; lv--)
    {
        pte_t *e = &pgtbl[VA_GET_PN(lv, va)];
        if ((*e) & PTE_FLAG_V)
            pgtbl = (pagetable_t)(PTE_PA(*e));
        else
        {
            if (!alloc || (pgtbl = (pagetable_t)kmem_alloc()) == 0)
                return NULL;
            memset(pgtbl, 0, PAGE_SIZE);
            *e = PA_PTE(pgtbl) | PTE_FLAG_V;
        }
    }
    return &pgtbl[VA_GET_PN(0, va)];
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

pagetable_t vm_make_kernel_pagetable(void)
{
    pagetable_t tbl = (pagetable_t)kmem_alloc();

    // Kernel text;
    vm_mappages(tbl, (uint64)KERNEL_TEXT_BEGIN, (uint64)KERNEL_TEXT_BEGIN, ((uint64)KERNEL_TEXT_END) - ((uint64)KERNEL_TEXT_BEGIN), PTE_FLAG_R | PTE_FLAG_X);
    // Kernel data;
    vm_mappages(tbl, (uint64)KERNEL_TEXT_END, (uint64)KERNEL_TEXT_END, (uint64)KERNEL_END - (uint64)KERNEL_TEXT_END, PTE_FLAG_R | PTE_FLAG_W);
    // Kernel memory pool;
    vm_mappages(tbl, PAGE_ROUND_UP((uint64)KERNEL_END), PAGE_ROUND_UP((uint64)KERNEL_END), KMEM_SIZE, PTE_FLAG_R | PTE_FLAG_W);
    return tbl;
}

void vm_kernel_init(void)
{
    kernel_pagetable = vm_make_kernel_pagetable();
}

void vm_hart_enable(void)
{
    sfence_vma();
    w_satp(SATP(VMEM_MODE, kernel_pagetable));
    sfence_vma();
}