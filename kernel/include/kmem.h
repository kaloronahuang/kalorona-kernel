// kmem.h
#ifndef KMEM

#define KMEM

#include <asm/spriv.h>
#include <asm/scsr.h>
#include <types.h>
#include <parameters.h>
#include <vmem.h>
#include <utilities/algorithm.h>
#include <bootmem.h>
#include <lock.h>

#define MAX_MEM_ORDER 11

struct kmem_free_area_struct
{
    struct list_node free_page_list;
    uint8 *free_area_map;
    size_t free_area_map_size;
};

struct kmem_node_struct
{
    // multi hart core may use the lock;
    struct spinlock lock;
    // info;
    ulong begin_ppn;
    size_t num_ppn;
    uint8 *mem_map;
    size_t mem_map_size;
    // for the low level API;
    struct kmem_free_area_struct free_areas[MAX_MEM_ORDER];
};

extern struct kmem_node_struct kmem;

void kmem_init(void);

// low level API for allocating pages;
void *kmem_alloc_pages(size_t page_count);
void kmem_free_pages(void *addr, size_t page_count);

// high level API;
void *kmem_alloc(size_t size);
void kmem_free(void *addr);

#endif