// entrypoint.c
#include <types.h>
#include <parameters.h>
#include <console.h>
#include <asm/sbi.h>
#include <asm/csr.h>
#include <asm/registers.h>

__attribute__((aligned(16))) char init_stack[4096 * MAX_CPU];

void kernel_entrypoint(uint64 hartid, uint64 fdt_addr)
{
    w_tp(hartid);
    sbi_srst_system_reset(0, 0);
}
