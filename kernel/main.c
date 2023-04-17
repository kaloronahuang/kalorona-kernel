// main.c
#include <types.h>
#include <parameters.h>

#include <asm/sbi.h>
#include <asm/registers.h>
#include <utilities/bytes.h>
#include <utilities/string.h>

#include <boot.h>
#include <console.h>
#include <kmem.h>
#include <proc.h>
#include <vmem.h>
#include <kimage_defs.h>
#include <device.h>
#include <device/memory.h>
#include <signal.h>
#include <bootmem.h>

__attribute__((aligned(4096))) char vkernel_stack[VKERNEL_STACK_SIZE * MAX_CPU];

struct boot_command vboot_cmd;

void parse_boot_args()
{
    /*
        Regular arguments:
        kernel.elf [options]
        options:
            --fdt [fdt_address]: Flatten Device Tree at [fdt_address], required;
    */
    char *ptr = vboot_cmd.argv;
    for (int arg = 0; arg < vboot_cmd.argc; arg++)
    {
        if (strcmp(ptr, "--fdt") == 0 && ++arg < vboot_cmd.argc)
        {
            ptr += strlen(ptr) + 1;
            flatten_device_tree = (struct fdt_header *)strtoul(ptr, NULL, 16);
        }
        else
            panic("parse_boot_args");
    }
}

void kernel_main(int argc, char *const argv[])
{
    // copy the arguments;
    size_t argv_ptr = 0;
    vboot_cmd.argc = argc;
    for (int i = 0; i < argc; i++)
    {
        size_t ptr = 0;
        do
        {
            vboot_cmd.argv[argv_ptr++] = argv[i][ptr];
        } while (argv[i][ptr++] != '\0');
    }
    parse_boot_args();
    // initialize;
    console_init();
    printf("[kernel]vkernel running\n");
    printf("[kernel]boot argc: %d\n", argc);
    for (int i = 0, j = 0; i < vboot_cmd.argc; i++)
    {
        printf("[kernel]- \"%s\"\n", vboot_cmd.argv + j);
        j += strlen(vboot_cmd.argv + j) + 1;
    }
    // load the fdt;
    device_fdt_init(flatten_device_tree);
    // init memory and setup bootmem;
    device_memory_init();
    bootmem_init();
    // setup the kmem;
    kmem_init();
    // retire the bootmem;
    // setup new vmem structure;
    // construct user mode;
    // filesystem;
    // load drivers;
    // scheduler;
    // debug use;
    sbi_srst_system_reset(0, 0);
}