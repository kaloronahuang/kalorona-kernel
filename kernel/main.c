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
#include <device/fdt.h>
#include <utilities/bytes.h>

void vm_test();

void vm_test_piror();

void kernel_main()
{
    if (booted && boot_hartid == r_tp())
    {
        // Console;
        console_init();
        print_str("Kalorona Kernel\n");
        print_str("Console initialized.\n");
        print_str("Current hartid: "), print_uint64(boot_hartid), print_str("\n\n");

        // Device Tree;
        print_str("Device Tree\n");
        print_str("- fdt: (hex address) "), print_uint64_by_base((uint64)fdt, 16), print_str("\n");
        print_str("- size: (dec size) "), print_uint64(flip_bytes_32(fdt->totalsize)), print_str("\n");

        // Kernel Memory;
        kmem_init();
        print_str("Kernel memory initialized.\n");
        print_str("- kernel memory head: (hex address) "), print_uint64_by_base((uint64)(kmem_head), 16), print_str("\n");
        print_str("- kernel memory size: (dec size) "), print_uint64(KMEM_SIZE), print_str("\n");
        print_str("- kernel memory page count: (dec count) "), print_uint64(KMEM_PAGE_COUNT), print_str("\n\n");

        // Virtual Memory;
        vm_kernel_init();
        print_str("Virtual memory tables for kernel initialized.\n\n");

        // Other harts;
        print_str("Starting other harts...\n");
        hart_count = 1;
        for (ulong hid = 0; hid < MAX_CPU; hid++)
            if (hid != boot_hartid)
            {
                struct sbiret res = sbi_hsm_hart_start(hid, (ulong)(&__entry), (ulong)(fdt));
                if (res.error == 0)
                    hart_count++;
            }
        print_str("Core Count: "), print_uint64(hart_count), print_str("\n\n");
    }
    vm_hart_enable();
}