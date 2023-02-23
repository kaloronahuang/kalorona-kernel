// entrypoint.c
#include <types.h>
#include <parameters.h>
#include <boot.h>
#include <asm/registers.h>
#include <console.h>

void kernel_main();

__attribute__((aligned(16))) char init_stack[4096 * MAX_CPU];

ulong boot_hartid;
int booted = 0;

void kernel_entrypoint(ulong hartid, ptr_t fdt_addr)
{
    // Memorize the hartid and fdt_addr;
    print_str("HART STARTED");
    w_tp(hartid);
    fdt_addr = fdt_addr;
    if (booted == 0)
        booted = 1, boot_hartid = hartid;
    kernel_main();
}
