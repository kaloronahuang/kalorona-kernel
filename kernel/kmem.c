// kmem.c
// kmem managaes all available physical memory pages through PMA in virtual memory mode;
#include <kmem.h>
#include <lock.h>
#include <signal.h>
#include <utilities/string.h>
#include <device/memory.h>
#include <kimage_defs.h>
#include <console.h>
#include <vmem.h>

struct kmem_node_struct kmem;

static void kmem_page_manager_init(void)
{
    printf("[kmem_page_manager]initializing\n");
    for (int ord = 0; ord < MAX_MEM_ORDER; ord++)
    {
        kmem.free_areas[ord].free_area_map_size = ((kmem.num_ppn >> (ord + 1)) + 8 - 1) >> 3;
        kmem.free_areas[ord].free_area_map = bootmem_alloc(kmem.free_areas[ord].free_area_map_size, sizeof(uint8), 0);
    }
}

void kmem_init(void)
{
    // init the kmem info;
    // copy these from bootmem;
    printf("[kmem]initializing\n");
    kmem.begin_ppn = uma_node.begin_ppn;
    kmem.num_ppn = uma_node.num_ppn;
    kmem.mem_map = uma_node.mem_map;
    kmem.mem_map_size = uma_node.mem_map_size;
    // init buddy;
    kmem_page_manager_init();
    // make the kmem locked;
    spinlock_init(&kmem.lock, "kmem.lock");
}

// low level API for allocating pages;
void *kmem_alloc_pages(size_t page_count)
{

}

void kmem_free_pages(void *addr, size_t page_count)
{

}
