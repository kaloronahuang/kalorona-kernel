// entrypoint.c
// To create the temporary pagetable and switch to the vkernel;
#include <asm/scsr.h>
#include <asm/spriv.h>
#include <asm/registers.h>
#include <types.h>
#include <parameters.h>
#include <boot.h>
#include <vmem_defs.h>
#include <vmem.h>

__attribute__((aligned(16))) char init_stack[4096];
__attribute__((aligned(4096))) pte_t pkernel_pgtbl[1 << 9];
extern void (*vkernel_main)(int, char *const *);

// Read arguments from the bootloader, and start kernel booting;
ulong kernel_entrypoint(int argc, char *const argv[])
{
    // Prepare the MMU - Temperary mapping;
    // - Identity mapping;
    // - Switch to virtual memory;
    // - Handover to vkernel;

    // Identity mapping;
    // Physical identity mapping;
    for (uint64 high_vpn = 0; high_vpn != (1ul << 8); high_vpn++)
    {
        pte_t pte = (PTE_FLAG_V | PTE_FLAG_R | PTE_FLAG_W | PTE_FLAG_X);
        uint64 p_addr = (high_vpn << ((VA_N_VPN - 1) * 9 + PAGE_OFFSET_SHIFT));
        pte |= PA_PTE(p_addr);
        pkernel_pgtbl[high_vpn] = pte;
    }
    // Virtual identity mapping;
    for (uint64 high_vpn = 0; high_vpn != (1ul << 8); high_vpn++)
    {
        pte_t pte = (PTE_FLAG_V | PTE_FLAG_R | PTE_FLAG_W | PTE_FLAG_X);
        uint64 p_addr = (high_vpn << ((VA_N_VPN - 1) * 9 + PAGE_OFFSET_SHIFT));
        pte |= PA_PTE(p_addr);
        pkernel_pgtbl[high_vpn | (1ul << 8)] = pte;
    }
    // Switch to virtual memory;
    sfence_vma();
    w_satp(SATP(VMEM_MODE, pkernel_pgtbl));
    // Handover to vkernel;
    vkernel_main(argc, argv);
    return 0;
}
