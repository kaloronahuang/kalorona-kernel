// entrypoint.c
#include <types.h>
#include <parameters.h>
#include <console.h>
#include <utilities/string.h>

__attribute__((aligned(16))) char init_stack[4096 * MAX_CPU];

void kernel_entrypoint(ulong hartid, ulong fdt_addr)
{
}
