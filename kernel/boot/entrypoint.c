// entrypoint.c
#include <types.h>
#include <parameters.h>
#include <boot.h>
#include <asm/registers.h>
#include <asm/sbi.h>
#include <console.h>
#include <device.h>

__attribute__((aligned(16))) char init_stack[4096 * MAX_CPU];

ulong boot_hartid;
int booted;
struct fdt_header *flatten_device_tree;

void kernel_entrypoint(ulong hartid, struct fdt_header *fdt_addr)
{
    // Memorize the hartid and fdt_addr;
    w_tp(hartid);
    if (booted == 0)
        booted = 1, boot_hartid = hartid, flatten_device_tree = fdt_addr;
    kernel_main();
}
