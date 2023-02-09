// entrypoint.c
#include <types.h>
#include <parameters.h>
#include <riscv/sbi.h>
#include <utilities/string.h>

__attribute__((aligned(16))) char init_stack[4096 * MAX_CPU];

void println(const char *s)
{
    size_t slen = strlen(s);
    for (int i = 0; i < slen; i++)
        sbi_legacy_console_putchar(s[i]);
    sbi_legacy_console_putchar('\n');
}

void kernel_entrypoint(ulong hartid, ulong fdt_addr)
{
    while (1)
    {
        char *s = "Hello World from Kalorona-Kernel";
        println(s);
        sbi_srst_system_reset(0, 0);
    }
}
