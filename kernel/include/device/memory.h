// memory.h
#ifndef DEVICE_MEMORY

#define DEVICE_MEMORY

#include <device.h>

#define MAX_RAM_SEGMENT 1024

struct ram_segment_descriptor_t
{
    uint64 pa_begin, pa_len;
};

struct ram_descriptor_t
{
    struct ram_segment_descriptor_t ram;
    size_t available_ram_segments_num;
    struct ram_segment_descriptor_t available_ram_segments[MAX_RAM_SEGMENT];
};

extern struct ram_descriptor_t ram_descriptor;

void device_memory_init(struct fdt_header *fdt);

#endif