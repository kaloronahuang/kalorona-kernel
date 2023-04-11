// bootmem.c
#include <bootmem.h>
#include <device/memory.h>
#include <utilities/bytes.h>
#include <vmem.h>
#include <signal.h>

struct bootmem_node_struct uma_node;

static void bootmem_init_uma_node()
{
    // read the device memory info;
    // get the memory upper bound & lower bound;
    ulong upper_ppn = 0;
    ulong lower_ppn = (ulong)(-1);
    for (int i = 0; i < ram_descriptor.available_ram_segments_num; i++)
    {
        ulong beg_paddr = ram_descriptor.available_ram_segments[i].pa_begin;
        ulong end_paddr = beg_paddr + ram_descriptor.available_ram_segments[i].pa_len;

        ulong beg_ppn = (PAGE_ROUND_UP(beg_paddr) >> PAGE_SHIFT);
        ulong end_ppn = (PAGE_ROUND_DOWN(end_paddr) >> PAGE_SHIFT);

        upper_ppn = (upper_ppn < end_ppn) ? end_ppn : upper_ppn;
        lower_ppn = (lower_ppn > beg_ppn) ? beg_ppn : lower_ppn;
    }
    uma_node.begin_ppn = lower_ppn;
    uma_node.num_ppn = upper_ppn - lower_ppn;
    // mem_map size;
    uma_node.mem_map_size = ((uma_node.num_ppn + 0x7) >> 3);
    // binding the mem_map at the beginning of the memory;
    uma_node.mem_map = PMA_PA2VA(uma_node.begin_ppn << PAGE_SHIFT);
    // set all to reserved;
    memset(uma_node.mem_map, 0xff, sizeof(uint8) * uma_node.mem_map_size);
}

static void bootmem_free_available_segments()
{
    for (int i = 0; i < ram_descriptor.available_ram_segments_num; i++)
        bootmem_free(ram_descriptor.available_ram_segments[i].pa_begin,
                     ram_descriptor.available_ram_segments[i].pa_len);
}

void bootmem_init(void)
{
    // init the uma node;
    bootmem_init_uma_node();
    // free all available space;
    bootmem_free_available_segments();
    // mark the mem_map area reserved;
    bootmem_reserve(PMA_VA2PA(uma_node.mem_map), uma_node.mem_map_size);
}

void bootmem_reserve(ulong paddr, size_t size)
{
    ulong beg_ppn = PAGE_ROUND_DOWN(paddr) >> PAGE_SHIFT;
    ulong end_ppn = PAGE_ROUND_UP(paddr + size) >> PAGE_SHIFT;
    if (beg_ppn < uma_node.begin_ppn)
        panic("bootmem_reserve - begin_ppn error");
    if (end_ppn > uma_node.begin_ppn + uma_node.num_ppn)
        panic("bootmem_reserve - end_ppn error");
    ulong bidx = beg_ppn - uma_node.begin_ppn;
    ulong eidx = end_ppn - uma_node.begin_ppn;
    for (ulong i = bidx; i != eidx; i++)
        if (test_and_set_bit(uma_node.mem_map, i))
            panic("bootmem_reserve - reserved twice");
}

static ulong bootmem_scan_for_available_slot(ulong beg_idx, ulong step, size_t slot_size)
{
    ulong eidx = uma_node.num_ppn;
    for (ulong pos = beg_idx; pos < eidx; pos += step)
    {
        if (test_bit(uma_node.mem_map, pos))
            continue;
        bool flag = true;
        for (ulong j = pos + 1; j < pos + slot_size; j++)
        {
            if (j >= eidx)
                return NULL;
            if (test_bit(uma_node.mem_map, j))
                break;
        }
    }
}

void *bootmem_alloc(size_t size, ulong alignment, ulong goal)
{
    // check the arguments;
    if (size == 0)
        return NULL;
    if (alignment & (alignment - 1))
        return NULL;
    // to satisfy the alignment;
    ulong offset = 0;
    if (alignment)
        offset = ((alignment - ((uma_node.begin_ppn << PAGE_SHIFT) & (alignment - 1))) & (alignment - 1));
    ulong step = alignment >> PAGE_SHIFT;
    if (step == 0)
        step = 1;
    // find somewhere available;
    // preferred;
}

void bootmem_free(ulong paddr, size_t size)
{
    ulong beg_ppn = PAGE_ROUND_DOWN(paddr) >> PAGE_SHIFT;
    ulong end_ppn = PAGE_ROUND_UP(paddr + size) >> PAGE_SHIFT;
    if (beg_ppn < uma_node.begin_ppn)
        panic("bootmem_free - begin_ppn error");
    if (end_ppn > uma_node.begin_ppn + uma_node.num_ppn)
        panic("bootmem_free - end_ppn error");
    ulong bidx = beg_ppn - uma_node.begin_ppn;
    ulong eidx = end_ppn - uma_node.begin_ppn;
    for (ulong i = bidx; i != eidx; i++)
        if (!test_and_clear_bit(uma_node.mem_map, i))
            panic("bootmem_free - freed twice");
}