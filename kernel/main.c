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
#include <trap.h>

__attribute__((aligned(4096))) char vkernel_stack[VKERNEL_STACK_SIZE * MAX_CPU];

struct boot_command vboot_cmd;

static struct spinlock started_hart_count_lock;
static int started_hart_count;
static int boot_hart_id;
static bool is_idmap_removed;

extern void __entry(void);

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
    if (started_hart_count == 0)
    {
        started_hart_count++;
        spinlock_init(&started_hart_count_lock, "");
        boot_hart_id = r_tp();
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
        // setup the vmem;
        vm_kernel_init();
        // construct user mode;
        proc_init();
        // trap handler install;
        ktrap_install_handler();
        // filesystem;
        // load drivers;
        // multicore activating;
        struct sbiret ret;
        for (int id = 0; id < MAX_CPU; id++)
        {
            ret = sbi_hsm_hart_get_status((ulong)id);
            if (ret.error != 0)
                break;
            hart_count++;
        }
        for (int id = 0; id < MAX_CPU; id++)
        {
            ret = sbi_hsm_hart_get_status((ulong)id);
            if (ret.error == 0 && ret.value == 1)
                ret = sbi_hsm_hart_start(id, (ulong)pkernel_hsm_entry, 0);
            else if (ret.error != 0)
                break;
        }
    }
    else
    {
        // trap handler install;
        ktrap_install_handler();
        spinlock_acquire(&started_hart_count_lock);
        started_hart_count++;
        spinlock_release(&started_hart_count_lock);
    }

    // wait for all to bootup;
    while (hart_count != started_hart_count)
        ;

    // disable identical mapping;
    if (r_tp() == boot_hart_id)
        vm_kernel_remove_idmap(), is_idmap_removed = true;
    else
        while (!is_idmap_removed)
            ;
    sfence_vma();

    printf("[kernel] hart #%d ready\n", (int)r_tp());

    // scheduling;
    while (true)
        ;
}