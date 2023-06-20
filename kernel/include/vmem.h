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

#define VA_USER_BEGIN 0x200000

#if VMEM_MODE == VMEM_MODE_SV39

// SV39 - General;
#define VA_LEN VMEM_SV39_LEN
#define VA_N_VPN VMEM_SV39_N_VPN

// SV39 - Virtual Memory Layout;
#define VA_USER_USER_HANDLER_BEGIN 0x3FFFFF0000
#define VA_USER_TRAPFRAME_BEGIN 0x3FFFFF8000
#define VA_KERNEL_BEGIN 0xFFFFFFC000000000

#define VA_KERNEL_PMA_BEGIN VA_KERNEL_BEGIN
#define VA_KERNEL_PMA_SIZE 0x2000000000

// SV39 - 128GB Physical Memory Supported;

#elif VMEM_MODE == VMEM_MODE_SV48

// SV48 - General;
#define VA_LEN VMEM_SV48_LEN
#define VA_N_VPN VMEM_SV48_N_VPN

// SV48 - Virtual Memory Layout;
#define VA_USER_USER_HANDLER_BEGIN 0x7FFFFFFF0000
#define VA_USER_TRAPFRAME_BEGIN 0x7FFFFFFF8000
#define VA_KERNEL_BEGIN 0xFFFF800000000000

#define VA_KERNEL_PMA_BEGIN VA_KERNEL_BEGIN
#define VA_KERNEL_PMA_SIZE 0x400000000000

#elif VMEM_MODE == VMEM_MODE_SV57

// SV57 - General;
#define VA_LEN VMEM_SV57_LEN
#define VA_N_VPN VMEM_SV57_N_VPN

// SV57 - Virtual Memory Layout;
#define VA_USER_USER_HANDLER_BEGIN 0xFFFFFFFFFF0000
#define VA_USER_TRAPFRAME_BEGIN 0xFFFFFFFFFF8000
#define VA_KERNEL_BEGIN 0x100000000000000

#define VA_KERNEL_PMA_BEGIN VA_KERNEL_BEGIN
#define VA_KERNEL_PMA_SIZE 0x80000000000000

#endif

#define VA_USER_USER_HANDLER_SIZE 0x8000
#define VA_USER_TRAPFRAME_SIZE 0x8000

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
#define PA_PTE(pa) ((((ulong)(pa)) >> PAGE_OFFSET_WIDTH) << PTE_FLAGS_WIDTH)
#define PTE_PA(pte) ((((ulong)(pte)) >> PTE_FLAGS_WIDTH) << PAGE_OFFSET_WIDTH)
#define SATP(mode, pgtbl) ((((ulong)mode) << VMEM_MODE_SHIFT) | (((ulong)pgtbl) >> PAGE_OFFSET_WIDTH))

#ifndef __ASSEMBLER__

#include <types.h>
#include <kmem.h>
#include <parameters.h>

#define PMA_VA2PA(va) (((ulong)(va)) & (~VA_KERNEL_BEGIN))
#define PMA_PA2VA(pa) (((ulong)(pa)) | VA_KERNEL_BEGIN)

typedef ulong pte_t;
typedef ulong *pagetable_t;

// Utilities functions for VM;

// query for physical addr;
pte_t *vm_walk(pagetable_t pgtbl, ulong va, int alloc);
int vm_mappages(pagetable_t pgtbl, ulong va, ulong pa, size_t siz, ulong flags);
void vm_unmappages(pagetable_t pgtbl, ulong va, size_t page_count, int do_free);
// entries have to be cleared before reaping pagetable;
void vm_reap_pagetable(pagetable_t pgtbl);
// destory the pagetable and its entries;
void vm_reap_pagetable_force(pagetable_t pgtbl);
void vm_map_user_handler(pagetable_t pgtbl);
void vm_unmap_user_handler(pagetable_t pgtbl);

// Kernel VM;

extern pagetable_t kernel_pagetable;
// vm_kernel_init translates the address through PMA;
void vm_kernel_init(void);
void vm_kernel_remove_idmap(void);
void vm_hart_enable(void);

// User VM;

pagetable_t vm_user_make_pagetable(void);
void *vm_translate(pagetable_t pgtbl, void *addr);
void vm_uvmcpy(pagetable_t pgtbl, pagetable_t new_pgtbl, int overwite);
void vm_memcpyin(pagetable_t pgtbl, void *dst, void *vsrc, size_t size);
void vm_memcpyout(pagetable_t pgtbl, void *vdst, void *src, size_t size);

#endif

#endif
