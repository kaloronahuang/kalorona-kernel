// fdt.h
#include <types.h>

#ifndef DEVICE_FDT

#define DEVICE_FDT

struct fdt_header
{
    uint32 magic;
    uint32 totalsize;
    uint32 off_dt_struct;
    uint32 off_dt_strings;
    uint32 off_mem_rsvmap;
    uint32 version;
    uint32 last_comp_version;
    uint32 boot_cpuid_phys;
    uint32 size_dt_strings;
    uint32 size_dt_struct;
};

extern struct fdt_header *fdt;

#endif