// vmem.h
#include <types.h>
#include <kmem.h>
#include <parameters.h>
#include <vmem_defs.h>

#ifndef VMEM

#define VMEM

typedef uint64 pte_t;
typedef uint64 *pagetable_t;

// PTE Flags;
#define PTE_FLAG_V (1ULL << 0)
#define PTE_FLAG_R (1ULL << 1)
#define PTE_FLAG_W (1ULL << 2)
#define PTE_FLAG_X (1ULL << 3)
#define PTE_FLAG_U (1ULL << 4)

#define PTE_SHIFT 10

#define PAGE_OFFSET_SHIFT 12

#define VA_GET_PN(lv, va) (((va) >> (9 * (lv) + PAGE_OFFSET_SHIFT)) & 0x1FF)
#define PA_PTE(pa) ((((uint64)(pa)) >> PAGE_OFFSET_SHIFT) << PTE_SHIFT)
#define PTE_PA(pte) ((((uint64)(pte)) >> PTE_SHIFT) << PAGE_OFFSET_SHIFT)

#define SATP(mode, pgtbl) (((mode) << 60) | (((uint64)pgtbl) >> PAGE_OFFSET_SHIFT))

#define MAX_VA (~(0ULL))

pte_t *vm_walk(pagetable_t pgtbl, uint64 va, int alloc);
void vm_freewalk(pagetable_t pgtbl);
int vm_mappages(pagetable_t pgtbl, uint64 va, uint64 pa, size_t siz, uint64 flags);
void vm_hart_enable(void);

pagetable_t vm_kernel_make_pagetable(void);
void vm_kernel_init(void);

pagetable_t vm_user_make_pagetable(void);
uint64 vm_user_walk_addr(pagetable_t pgtbl, uint64 va);
void vm_unmappages(pagetable_t pgtbl, uint64 va, size_t page_count, int do_free);
void vm_user_free_pagetable(pagetable_t pgtbl);

#endif