// setup_mmu.c
#define PKERNEL

#include <vmem.h>
#include <kimage_defs.h>

__attribute__((aligned(4096))) __attribute__((section(".boot.bss"))) pte_t pkernel_pgtbl[VA_N_VPN - 2][1 << 9];

__attribute__((section(".boot.text"))) static inline void __boot_w_satp(uint64 x)
{
    asm volatile("csrw satp, %0"
                 :
                 : "r"(x));
}

__attribute__((section(".boot.text"))) static inline void __boot_sfence_vma()
{
    asm volatile("sfence.vma zero, zero");
}

__attribute__((section(".boot.text"))) void setup_mmu(void)
{
    // keep the physical memory at user address space;
    for (uint64 phy_pn = 0; phy_pn != (1ul << 8); phy_pn++)
    {
        pte_t pte = (PTE_FLAG_V | PTE_FLAG_R | PTE_FLAG_W | PTE_FLAG_X);
        uint64 p_addr = (phy_pn << ((VA_N_VPN - 1) * PN_WIDTH + PAGE_OFFSET_WIDTH));
        pte |= PA_PTE(p_addr);
        pkernel_pgtbl[VA_N_VPN - 3][phy_pn] = pte;
    }
    
    // setup Physical Memory Access (PMA);
    for (uint64 phy_pn = 0; phy_pn != (1ul << 7); phy_pn++)
    {
        pte_t pte = (PTE_FLAG_V | PTE_FLAG_R | PTE_FLAG_W | PTE_FLAG_X);
        uint64 p_addr = (phy_pn << ((VA_N_VPN - 1) * PN_WIDTH + PAGE_OFFSET_WIDTH));
        pte |= PA_PTE(p_addr);
        pkernel_pgtbl[VA_N_VPN - 3][phy_pn | (1ul << 8)] = pte;
    }

    // connecting high pagtables;
    for (uint64 i = VA_N_VPN - 3; i != 0; i--)
    {
        uint64 pn = 0x1FF;
        pte_t pte = PTE_FLAG_V;
        pte |= PA_PTE(pkernel_pgtbl[i - 1]);
        pkernel_pgtbl[i][pn] = pte;
    }

    // map the kernel image pa to kernel image va on low pagetable;
    uint64 kimage_pa = (uint64)__KERNEL_IMG_PA_BEGIN;
    uint64 kimage_va = (uint64)__KERNEL_IMG_VA_BEGIN;
    uint64 kimage_va_end = (uint64)__KERNEL_IMG_VA_END;
    uint64 step = 1ul << (PAGE_OFFSET_WIDTH + PN_WIDTH * 2);

    while (kimage_va < kimage_va_end)
    {
        pte_t pte = (PTE_FLAG_V | PTE_FLAG_R | PTE_FLAG_W | PTE_FLAG_X);
        pte |= PA_PTE(kimage_pa);
        pkernel_pgtbl[0][VA_GET_PN(2, kimage_va)] = pte;

        kimage_va += step;
        kimage_pa += step;
    }

    // switch to virtual memory;
    uint64 new_satp = SATP(VMEM_MODE, pkernel_pgtbl[VA_N_VPN - 3]);
    __boot_sfence_vma();
    __boot_w_satp(new_satp);
    __boot_sfence_vma();
}