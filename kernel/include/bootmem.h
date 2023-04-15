// bootmem.h
#ifndef BOOTMEM

#define BOOTMEM

#include <types.h>

// Page definitions;
#define PAGE_SIZE 0x1000ULL
#define PAGE_SHIFT 12

#define BINARY_ROUND_UP(x, agn) (((x) + (agn) - 1) & (~(agn - 1)))
#define BINARY_ROUND_DOWN(x, agn) ((x) & (~(agn - 1)))

#define PAGE_ROUND_UP(x) (((x) + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1)))
#define PAGE_ROUND_DOWN(x) ((x) & (~(PAGE_SIZE - 1)))

struct bootmem_node_struct
{
    ulong begin_ppn;
    size_t num_ppn;
    uint8 *mem_map;
    size_t mem_map_size;
    // the last page that is used in allocation;
    ulong last_page;
    // the first available offset in the last page;
    ulong last_page_offset;
    // no need for spinlock to preserve the concurrency;
    // single core mode during bootmem mode;
};

extern struct bootmem_node_struct uma_node;

void bootmem_init(void);

void bootmem_reserve(ulong paddr, size_t size);

void *bootmem_alloc(size_t size, ulong alignment, ulong goal);

void bootmem_free(ulong paddr, size_t size);

#endif