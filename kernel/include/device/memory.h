// memory.h
#ifndef DEVICE_MEMORY

#define DEVICE_MEMORY

#include <device.h>

extern const char memory_fdt_node_name_prefix[];

void device_memory_init_from_fdt(struct fdt_header *fdt);

#endif