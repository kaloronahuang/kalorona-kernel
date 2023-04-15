// bootmem.c
#include <bootmem.h>
#include <device/memory.h>
#include <utilities/bytes.h>
#include <vmem.h>
#include <signal.h>
#include <console.h>

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

static void bootmem_test(void)
{
    printf("[bootmem_test]running test\n");

}

void bootmem_init(void)
{
    printf("[bootmem]setting up the bootmem on UMA machine\n");
    // init the uma node;
    bootmem_init_uma_node();
    // free all available space;
    bootmem_free_available_segments();
    // mark the mem_map area reserved;
    bootmem_reserve(PMA_VA2PA(uma_node.mem_map), uma_node.mem_map_size);
    // debugging test;
    bootmem_test();
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

void *bootmem_alloc(size_t size, ulong alignment, ulong goal)
{
    // check the arguments;
    if (size == 0)
        return NULL;
    if (alignment & (alignment - 1))
        return NULL;
    // to satisfy the alignment;
    ulong ppn_offset = 0;
    if (alignment)
        ppn_offset = (alignment - ((uma_node.begin_ppn << PAGE_SHIFT) & (alignment - 1))) & (alignment - 1);
    ppn_offset >>= PAGE_SHIFT;
    // find somewhere available;
    // - satisfy the goal first;
    ulong preferred = 0;
    if (goal && (goal >> PAGE_SHIFT) >= uma_node.begin_ppn && (goal >> PAGE_SHIFT) < uma_node.begin_ppn + uma_node.num_ppn)
        preferred = goal - (uma_node.begin_ppn << PAGE_SHIFT);
    preferred = ((preferred + alignment - 1) & (~(alignment - 1))) >> PAGE_SHIFT;
    // added the base;
    preferred += ppn_offset;

    size_t area_size = (size + PAGE_SIZE - 1) >> PAGE_SHIFT;
    ulong step = alignment >> PAGE_SHIFT;
    if (step == 0)
        step = 1;
    // scan;
    ulong spos[2] = {preferred, ppn_offset};
    bool spot_found = false;
    ulong spot = 0;
    for (int b = 0; b < 2; b++)
    {
        for (ulong i = spos[b]; i < uma_node.num_ppn; i++)
        {
            bool found = true;
            if (test_bit(uma_node.mem_map, i))
                continue;
            for (ulong j = i + 1; j < i + area_size; j++)
                if (j >= uma_node.num_ppn || test_bit(uma_node.mem_map, j))
                {
                    found = false;
                    break;
                }
            if (found)
            {
                spot_found = true, spot = i;
                break;
            }
        }
        if (spot_found)
            break;
    }
    if (!spot_found)
        return NULL;
    // found;
    // two types: appending, assigning newer;
    ulong ret_addr = NULL;
    if (alignment <= PAGE_SIZE && uma_node.last_page_offset && uma_node.last_page + 1 == spot)
    {
        ulong offset = (uma_node.last_page_offset + alignment - 1) & (~(alignment - 1));
        size_t rem_size = PAGE_SIZE - offset;
        // no need to expand a new page;
        if (size < rem_size)
        {
            // no need for toggling up the bits;
            area_size = 0;
            ret_addr = offset + ((uma_node.last_page + uma_node.begin_ppn) << PAGE_SHIFT);
            ret_addr = PMA_PA2VA(ret_addr);
            uma_node.last_page_offset = offset + size;
        }
        // need to increment the last_page;
        else
        {
            size_t back_size = size - rem_size;
            area_size = (back_size + PAGE_SIZE - 1) >> PAGE_SHIFT;
            ret_addr = offset + ((uma_node.last_page + uma_node.begin_ppn) << PAGE_SHIFT);
            ret_addr = PMA_PA2VA(ret_addr);
            uma_node.last_page = spot + area_size - 1;
            uma_node.last_page_offset = back_size;
        }
        uma_node.last_page_offset &= (PAGE_SIZE - 1);
    }
    else
    {
        ret_addr =  ((spot + uma_node.begin_ppn) << PAGE_SHIFT);
        ret_addr = PMA_PA2VA(ret_addr);
        uma_node.last_page = spot + area_size - 1;
        uma_node.last_page_offset = (size & (PAGE_SIZE - 1));
    }
    for (ulong i = spot; i != spot + area_size; i++)
        if (test_and_set_bit(uma_node.mem_map, i))
            panic("bootmem_alloc");
    memset((void *)ret_addr, 0, size);
    return (void *)ret_addr;
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