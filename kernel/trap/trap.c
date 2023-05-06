// trap.c
#include <asm/scsr.h>
#include <vmem.h>
#include <trap.h>

extern void kernel_handler();

void ktrap_install_handler(void) { w_stvec((uint64)kernel_handler); }

void ktrap_handler(void)
{
    // TODO;
}

// jumping from user space (trapframe);
void utrap_handler(void)
{
    // TODO;
}
