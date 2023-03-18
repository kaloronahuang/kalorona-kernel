// memory.c
#include <device/memory.h>
#include <utilities/algorithm.h>
#include <kmem.h>
#include <device.h>
#include <kimage_defs.h>
#include <signal.h>

struct ram_descriptor_t ram_descriptor;

static uint64 __sort_seg_begin[MAX_RAM_SEGMENT], __sort_seg_end[MAX_RAM_SEGMENT], __sort_seg_num;

static void device_calc_available_ram()
{
    // to avoid the kernel image;
    __sort_seg_begin[__sort_seg_num] = ram_descriptor.ram.pa_begin;
    __sort_seg_end[__sort_seg_num] = KERNEL_IMG_PA_END;
    __sort_seg_num++;
    // align the memory page;
    for (int i = 0; i < __sort_seg_num; i++)
    {
        __sort_seg_begin[i] &= (~(PAGE_SIZE - 1));
        __sort_seg_end[i] = (__sort_seg_end[i] + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1));
    }
    // boundary;
    __sort_seg_begin[__sort_seg_num] = (ram_descriptor.ram.pa_begin + ram_descriptor.ram.pa_len) & (~(PAGE_SIZE - 1));
    __sort_seg_end[__sort_seg_num] = -1ull;
    __sort_seg_num++;
    // sort;
    qsort_uint64(__sort_seg_begin, __sort_seg_num);
    qsort_uint64(__sort_seg_end, __sort_seg_num);
    int bptr = 0, eptr = 0, acc = 0;
    uint64 last_ptr = ram_descriptor.ram.pa_begin;
    while (bptr < __sort_seg_num || eptr < __sort_seg_num)
    {
        uint64 pos;
        int val;
        if (bptr == __sort_seg_num)
            pos = __sort_seg_end[eptr++], val = -1;
        else if (eptr == __sort_seg_num)
            pos = __sort_seg_begin[bptr++], val = 1;
        else if (__sort_seg_begin[bptr] == __sort_seg_end[eptr])
        {
            uint64 x = __sort_seg_begin[bptr];
            val = 0;
            while (bptr < __sort_seg_num && __sort_seg_begin[bptr] == x)
                val++, bptr++;
            while (eptr < __sort_seg_num && __sort_seg_end[eptr] == x)
                val--, eptr++;
            pos = x;
        }
        else if (__sort_seg_begin[bptr] < __sort_seg_end[eptr])
            val = 1, pos = __sort_seg_begin[bptr++];
        else if (__sort_seg_begin[bptr] > __sort_seg_end[eptr])
            val = -1, pos = __sort_seg_end[eptr++];
        int new_acc = acc + val;
        if (new_acc == 0 && acc > 0)
            last_ptr = pos;
        else if (new_acc > 0 && acc == 0 && pos > last_ptr)
        {
            ram_descriptor.available_ram_segments[ram_descriptor.available_ram_segments_num].pa_begin = last_ptr;
            ram_descriptor.available_ram_segments[ram_descriptor.available_ram_segments_num].pa_len = pos - last_ptr;
            ram_descriptor.available_ram_segments_num++;
        }
        acc = new_acc;
    }
}

static void device_fdt_get_reg(struct fdt_header *fdt, int node_offset, uint64 *p1, uint64 *p2)
{
    struct fdt_property *reg_prop = fdt_get_property(fdt, node_offset, "reg", NULL);
    uint64 *reg_ptr = reg_prop->data;
    *p1 = fdt64_to_cpu(*reg_ptr);
    reg_ptr++;
    *p2 = fdt64_to_cpu(*reg_ptr);
}

static void device_walk_fdt_for_memory(struct fdt_header *fdt, int node_offset)
{
    if (strncmp(fdt_get_name(fdt, node_offset, NULL), "memory", strlen("memory") - 1) == 0)
        device_fdt_get_reg(fdt, node_offset, &ram_descriptor.ram.pa_begin, &ram_descriptor.ram.pa_len);
    else if (strncmp(fdt_get_name(fdt, node_offset, NULL), "reserved-memory", strlen("reserved-memory") - 1) == 0)
    {
        int res_mem_off;
        fdt_for_each_subnode(res_mem_off, fdt, node_offset)
        {
            // printf("resvm\n");
            uint64 res_beg, res_len;
            device_fdt_get_reg(fdt, res_mem_off, &res_beg, &res_len);
            __sort_seg_begin[__sort_seg_num] = res_beg;
            __sort_seg_end[__sort_seg_num] = res_beg + res_len;
            __sort_seg_num++;
        }
        return;
    }
    int subnode_offset;
    fdt_for_each_subnode(subnode_offset, fdt, node_offset)
        device_walk_fdt_for_memory(fdt, subnode_offset);
}

void device_memory_init(struct fdt_header *fdt)
{
    __sort_seg_num = 0;
    device_walk_fdt_for_memory(fdt, 0);
    printf("[device_memory]ram ranges from %p to %p\n", ram_descriptor.ram.pa_begin, ram_descriptor.ram.pa_begin + ram_descriptor.ram.pa_len - 1);
    device_calc_available_ram();
    printf("[device_memory]available memory segments:\n");
    for (int i = 0; i < ram_descriptor.available_ram_segments_num; i++)
        printf(
            "[device_memory]    [%d]%p - %p\n",
            i,
            ram_descriptor.available_ram_segments[i].pa_begin,
            ram_descriptor.available_ram_segments[i].pa_begin + ram_descriptor.available_ram_segments[i].pa_len - 1);
}