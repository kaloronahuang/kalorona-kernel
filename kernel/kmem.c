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
#include <utilities/bytes.h>

struct kmem_node_struct kmem;

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
    kmem.lock_installed = true;
}
