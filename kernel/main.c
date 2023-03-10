// main.c
#include <types.h>
#include <parameters.h>
#include <boot.h>
#include <asm/registers.h>
#include <asm/sbi.h>
#include <console.h>
#include <kmem.h>
#include <proc.h>
#include <vmem.h>
#include <utilities/bytes.h>

void vm_test();

void vm_test_piror();

void kernel_main()
{
    if (booted && boot_hartid == r_tp())
    {
        // Console;
        console_init();
        printf("Kalorona Kernel\n");
        printf("Console initialized.\n");
        printf("Current hartid: %u\n\n", (uint32)boot_hartid);

        // Device Tree;
        // printf("Device Tree\n");
        // printf("- fdt: (hex address) %p\n", fdt);
        // printf("- size: (dec size) %u\n\n", flip_bytes_32(fdt->totalsize));

        // Kernel Memory;
        kmem_init();
        printf("Kernel memory initialized.\n");
        printf("- kernel memory head: (hex address) %p\n", (uint64)kmem_head);
        printf("- kernel memory size: (dec size) %u\n", (uint32)(KMEM_SIZE));
        printf("- kernel memory page count: (dec count) %u\n\n", (uint32)(KMEM_PAGE_COUNT));

        // Virtual Memory;
        vm_kernel_init();
        printf("Virtual memory tables for kernel initialized.\n\n");

        // Other harts;
        printf("Starting other harts...\n");
        hart_count = 1;
        for (ulong hid = 0; hid < MAX_CPU; hid++)
            if (hid != boot_hartid)
            {
                struct sbiret res = sbi_hsm_hart_start(hid, (ulong)(&__entry), NULL /* (ulong)(fdt) */);
                if (res.error == 0)
                    hart_count++;
            }
        printf("Core Count: %u\n\n", (uint32)hart_count);
    }
    vm_hart_enable();
}