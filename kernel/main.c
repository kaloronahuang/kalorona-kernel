// main.c
#include <types.h>
#include <parameters.h>
#include <boot.h>
#include <asm/registers.h>
#include <asm/sbi.h>
#include <console.h>

void kernel_main()
{
    if (booted && boot_hartid == r_tp())
    {
        // init;
        init_console();
        // eventually, start all other harts;
        for (ulong hid = 0; hid < MAX_CPU; hid++)
            if (hid != boot_hartid)
                sbi_hsm_hart_start(hid, (ulong)(&__entry), FDT_ADDR);
    }
}