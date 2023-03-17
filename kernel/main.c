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
#include <device.h>

struct boot_command vboot_cmd;

void kernel_main(int argc, char *const argv[])
{
    /* copy the arguments; */
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
    /* Initialize; */
    console_init();
    printf("[kernel] vkernel running\n");
    printf("[kernel] boot argc: %d\n", vboot_cmd.argc);
    for (int i = 0, j = 0; i < vboot_cmd.argc; i++)
    {
        printf("[kernel] - \"%s\"\n", vboot_cmd.argv + j);
        j += strlen(vboot_cmd.argv + j) + 1;
    }
}