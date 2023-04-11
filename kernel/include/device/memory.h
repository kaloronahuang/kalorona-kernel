// memory.h
#ifndef DEVICE_MEMORY

#define DEVICE_MEMORY

#include <device.h>

#define MAX_RAM_SEGMENT 1024

struct ram_segment_descriptor_struct
{
    uint64 pa_begin, pa_len;
};

struct ram_descriptor_struct
{
    struct ram_segment_descriptor_struct ram;
    size_t available_ram_segments_num;
    struct ram_segment_descriptor_struct available_ram_segments[MAX_RAM_SEGMENT];
};

extern struct ram_descriptor_struct ram_descriptor;

void device_memory_init(void);

#endif