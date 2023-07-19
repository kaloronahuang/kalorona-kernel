// ns16550a.c
#include <device.h>
#include <kmem.h>

struct ns16550a_device_struct
{
};

bool ns16550a_driver_recognize_device(struct fdt_header *fdt, int node_offset)
{
    return (strcmp(fdt_get_name(fdt, node_offset, NULL), "uart") == 0 &&
            strcmp(fdt_get_property(fdt, node_offset, "compatible", NULL)->data, "ns16550a") == 0);
}

void ns16550a_driver_init(int devId, struct fdt_header *fdt, int node_offset)
{
    // register the device to device manager & HAL;
}

struct driver_struct ns16550a_driver = {
    .developer = "Kalorona Huang",
    .name = "NS16550A Compatible UART Driver",
    .version = 0,
    .init = ns16550a_driver_init,
    .recognize_device = ns16550a_driver_recognize_device};
