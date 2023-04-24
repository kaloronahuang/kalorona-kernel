// entrypoint.c
// To create the temporary pagetable and switch to the vkernel;
#define PKERNEL

#include <types.h>
#include <parameters.h>
#include <boot.h>

__attribute__((aligned(16))) __attribute__((section(".boot.bss"))) char init_stack[MAX_CPU * 512];

extern void setup_mmu(void);
extern void (*vkernel_entry)(int, char *const *);

__attribute__((section(".boot.text"))) ulong kernel_entrypoint(int argc, char *const argv[])
{
    // setup the temporary pagetable;
    setup_mmu();
    vkernel_entry(argc, argv);
    return 0;
}
