// device.c
#include <device.h>
#include <signal.h>
#include <console.h>
#include <device/memory.h>

struct fdt_header *flatten_device_tree;

// 4MB FDT Blob;
char fdt_blob[4 * (1 << 20)];

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

// Initialize devices other than memory;
void device_discover(void)
{
    struct fdt_header *fdt = flatten_device_tree;
}

// Initialize fdt and return a new fdt address;
struct fdt_header *device_fdt_init(struct fdt_header *fdt)
{
    printf("[device]flattened device tree detected\n");
    // check and brief the device tree;
    if (fdt_check_header(fdt) != 0)
        panic("device_init_by_fdt corrupt fdt");
    printf("[device]fdt addr: %p\n", fdt);
    printf("[device]fdt size: %u\n", (uint32)(fdt_totalsize(fdt)));
    printf("[device]fdt version: %u\n", (uint32)(fdt_version(fdt)));
    // copy it to the kernel memory;
    printf("[device]copying fdt to kernel, new addr: %p\n", fdt_blob);
    if ((uint32)(fdt_totalsize(fdt)) < sizeof(fdt_blob))
        memmove(fdt_blob, fdt, (uint32)(fdt_totalsize(fdt)));
    else
        panic("device_init_by_fdt insufficient space for fdt");
    fdt = (struct fdt_header *)fdt_blob;
    return fdt;
}

// Initialize the device manager and the memory;
void device_init(void)
{
}