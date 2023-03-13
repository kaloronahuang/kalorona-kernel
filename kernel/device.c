// device.c
#include <device.h>
#include <signal.h>
#include <console.h>
#include <device/memory.h>

static void device_init_node(struct fdt_header *fdt, int node_offset)
{

}

static void device_walk_fdt(struct fdt_header *fdt, int node_offset)
{
    device_init_node(fdt, node_offset);
    int subnode_offset;
    fdt_for_each_subnode(subnode_offset, fdt, node_offset)
        device_walk_fdt(fdt, subnode_offset);
}

void device_init_by_fdt(void)
{
    printf("[device]flattened device tree detected\n");
    // check and brief the device tree;
    struct fdt_header *fdt = flatten_device_tree;
    if (fdt_check_header(fdt) != 0)
        panic("device_init_by_fdt corrupt fdt");
    printf("[device]fdt addr: %p\n", fdt);
    printf("[device]fdt size: %u\n", (uint32)(fdt_totalsize(fdt)));
    printf("[device]fdt version: %u\n", (uint32)(fdt_version(fdt)));
    // init memory;
    device_memory_init_from_fdt(fdt);
    // walk other devices;
    device_walk_fdt(fdt, 0);
}

void device_init(void)
{
    device_init_by_fdt();
}