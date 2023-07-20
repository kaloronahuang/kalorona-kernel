// device.c
#include <device.h>
#include <signal.h>
#include <console.h>
#include <device/memory.h>
#include <kmem.h>

struct fdt_header *flatten_device_tree;

// 4MB FDT Blob;
char fdt_blob[4 * (1 << 20)];
dev_t device_counter;
struct device_manager_struct device_manager;

static struct device_struct *device_init_node(struct fdt_header *fdt, int node_offset)
{
    // finding compatible driver;
    for (struct driver_struct *drv = driver_manager.driver_list.next; drv != NULL; drv = drv->next)
        if (drv->recognize_device(fdt, node_offset))
        {
            struct device_struct *dev = drv->init(device_counter + 1, fdt, node_offset);
            if (dev != NULL)
                device_counter++;
            return dev;
        }
    return NULL;
}

static struct device_struct *device_walk_fdt(struct fdt_header *fdt, int node_offset)
{
    struct device_struct *dev = device_init_node(fdt, node_offset);
    if (dev == NULL)
    {
        // incompatible device;
        dev = (struct device_struct *)kmem_alloc(sizeof(struct device_struct));
        dev->devId = ++device_counter;
        dev->driver = NULL;
        dev->driver_internal = NULL;
        dev->name = "Unknown Device";
        dev->children = dev->next = dev->parent = NULL;
    }
    int subnode_offset = 0;
    fdt_for_each_subnode(subnode_offset, fdt, node_offset)
    {
        struct device_struct *child = device_walk_fdt(fdt, subnode_offset);
        child->parent = dev;
        child->next = dev->children;
        dev->children = child;
    }
    return dev;
}

// Initialize devices other than memory;
void device_discover(void)
{
    device_manager.root_dev.next = device_walk_fdt(flatten_device_tree, 0);
}

// Initialize fdt and return a new fdt address;
struct fdt_header *device_fdt_init(struct fdt_header *fdt)
{
    printf("[device]flattened device tree detected\n");
    // check and brief the device tree;
    if (fdt_check_header(fdt) != 0)
        panic("device_fdt_init corrupt fdt");
    printf("[device]fdt addr: %p\n", fdt);
    printf("[device]fdt size: %u\n", (uint32)(fdt_totalsize(fdt)));
    printf("[device]fdt version: %u\n", (uint32)(fdt_version(fdt)));
    // copy it to the kernel memory;
    printf("[device]copying fdt to kernel, new addr: %p\n", fdt_blob);
    if ((uint32)(fdt_totalsize(fdt)) < sizeof(fdt_blob))
        memmove(fdt_blob, fdt, (uint32)(fdt_totalsize(fdt)));
    else
        panic("device_fdt_init insufficient space for fdt");
    fdt = (struct fdt_header *)fdt_blob;
    return fdt;
}

// Initialize the device manager;
void device_init(void)
{
    device_counter = 0;
    // load drivers first;
    device_drivers_load();
    // then detect devices, invoke their initialization;
    device_discover();
    // and then activate HAL;
    hal_init();
}
