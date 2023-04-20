// vmem.h
// can be used in both pkernel and vkernel;

#ifndef VMEM

#define VMEM

#define VMEM_MODE_NONE 0

#define VMEM_MODE_SV39 8
#define VMEM_SV39_LEN 39
#define VMEM_SV39_N_VPN 3

#define VMEM_MODE_SV48 9
#define VMEM_SV48_LEN 48
#define VMEM_SV48_N_VPN 4

#define VMEM_MODE_SV57 10
#define VMEM_SV57_LEN 57
#define VMEM_SV57_N_VPN 5

#define VMEM_MODE_SHIFT 60

// By default, the mode is SV39;
// It can be altered by the macro option by the compiler;
#ifndef VMEM_MODE
#define VMEM_MODE VMEM_MODE_SV39
#endif

#define VA_USER_BEGIN 0x0

#if VMEM_MODE == VMEM_MODE_SV39

// SV39 - General;
#define VA_LEN VMEM_SV39_LEN
#define VA_N_VPN VMEM_SV39_N_VPN

// SV39 - Virtual Memory Layout;
#define VA_USER_KFRAME_BEGIN 0x3FFC000000
#define VA_KERNEL_BEGIN 0xFFFFFFC000000000

#define VA_KERNEL_PMA_BEGIN VA_KERNEL_BEGIN
#define VA_KERNEL_PMA_SIZE 0x2000000000

// SV39 - 128GB Physical Memory Supported;
// 4MB memory bitmap size;
#define MEM_BITMAP_SIZE 4194304

#elif VMEM_MODE == VMEM_MODE_SV48

// SV48 - General;
#define VA_LEN VMEM_SV48_LEN
#define VA_N_VPN VMEM_SV48_N_VPN

// SV48 - Virtual Memory Layout;
#define VA_USER_KFRAME_BEGIN 0x7FFFFC000000
#define VA_KERNEL_BEGIN 0xFFFF800000000000

#define VA_KERNEL_PMA_BEGIN VA_KERNEL_BEGIN
#define VA_KERNEL_PMA_SIZE 0x400000000000

#elif VMEM_MODE == VMEM_MODE_SV57

// SV57 - General;
#define VA_LEN VMEM_SV57_LEN
#define VA_N_VPN VMEM_SV57_N_VPN

// SV57 - Virtual Memory Layout;
#define VA_USER_KFRAME_BEGIN 0xFFFFFFFC000000
#define VA_KERNEL_BEGIN 0x100000000000000

#define VA_KERNEL_PMA_BEGIN VA_KERNEL_BEGIN
#define VA_KERNEL_PMA_SIZE 0x80000000000000

#endif

#define VA_USER_KFRAME_SIZE 0x4000000

#define VA_KERNEL_MOD_BEGIN 0xFFFFFFFF00000000
#define VA_KERNEL_IMG_BEGIN 0xFFFFFFFF80000000

// PTE;

#define PTE_FLAG_V (1ul << 0)
#define PTE_FLAG_R (1ul << 1)
#define PTE_FLAG_W (1ul << 2)
#define PTE_FLAG_X (1ul << 3)
#define PTE_FLAG_U (1ul << 4)

#define PTE_FLAGS_WIDTH 10
#define PAGE_OFFSET_WIDTH 12
#define PN_WIDTH 9

#define VA_GET_PN(lv, va) (((va) >> (PN_WIDTH * (lv) + PAGE_OFFSET_WIDTH)) & 0x1FF)
#define PA_PTE(pa) ((((uint64)(pa)) >> PAGE_OFFSET_WIDTH) << PTE_FLAGS_WIDTH)
#define PTE_PA(pte) ((((uint64)(pte)) >> PTE_FLAGS_WIDTH) << PAGE_OFFSET_WIDTH)
#define SATP(mode, pgtbl) ((((uint64)mode) << VMEM_MODE_SHIFT) | (((uint64)pgtbl) >> PAGE_OFFSET_WIDTH))

#ifndef __ASSEMBLER__

#include <types.h>
#include <kmem.h>
#include <parameters.h>

#define PMA_VA2PA(va) (((ulong)(va)) & (~VA_KERNEL_BEGIN))
#define PMA_PA2VA(pa) (((ulong)(pa)) | VA_KERNEL_BEGIN)

typedef uint64 pte_t;
typedef uint64 *pagetable_t;

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

#endif