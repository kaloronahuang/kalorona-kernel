// vmem.h
#include <types.h>
#include <kmem.h>
#include <parameters.h>

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

#define PAGE_OFFSET_SHIFT 12

#define VA_GET_PN(lv, va) (((va) >> (9 * (lv) + PAGE_OFFSET_SHIFT)) & 0x1FF)
#define PA_PTE(pa) ((((uint64)(pa)) >> PAGE_OFFSET_SHIFT) << 10)
#define PTE_PA(pte) ((((uint64)(pte)) >> 10) << PAGE_OFFSET_SHIFT)

#define SATP(mode, pgtbl) (((mode) << 60) | (((uint64)pgtbl) >> PAGE_OFFSET_SHIFT))

#if VMEM_MODE == 8L
#define VA_LEN 39
#elif VMEM_MODE == 9L
#define VA_LEN 48
#elif VMEM_MODE == 10L
#define VA_LEN 57
#elif VMEM_MODE == 11L
#define VA_LEN 64
#endif

#define MAX_VA (1UL << VA_LEN)

pte_t *vm_walk(pagetable_t pgtbl, uint64 va, int alloc);
int vm_mappages(pagetable_t pgtbl, uint64 va, uint64 pa, size_t siz, uint64 flags);
pagetable_t vm_make_kernel_pagetable(void);
void vm_kernel_init(void);
void vm_hart_enable(void);

#endif