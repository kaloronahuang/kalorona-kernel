// device.h
#ifndef DEVICE

#define DEVICE

#include <device/libfdt/libfdt.h>

extern struct fdt_header *flatten_device_tree;

struct fdt_header *device_fdt_init(struct fdt_header *fdt);

void device_discover(void);

void device_init(void);

#endif