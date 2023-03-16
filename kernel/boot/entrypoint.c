// entrypoint.c
#include <types.h>
#include <parameters.h>
#include <boot.h>
#include <asm/registers.h>

__attribute__((aligned(16))) char init_stack[4096 * MAX_CPU];

// Read arguments from the bootloader, and start kernel booting;
ulong kernel_entrypoint(int argc, char *const argv[])
{
    // kernel_main();
    // printf("%d %p\n", argc, argv[0]);
    // struct boot_command *ptr = vboot_cmd - ()
}
