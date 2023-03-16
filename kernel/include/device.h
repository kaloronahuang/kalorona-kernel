// device.h
#ifndef DEVICE

#define DEVICE

#include <device/libfdt/libfdt.h>

extern struct fdt_header *flatten_device_tree;

void device_init_by_fdt(struct fdt_header *fdt);

void device_init(void);

#endif