// kmem.h
#ifndef KMEM

#define KMEM

#include <asm/spriv.h>
#include <asm/scsr.h>
#include <types.h>
#include <parameters.h>
#include <vmem.h>
#include <utilities/algorithm.h>
#include <lock.h>

// Page definitions;
#define PAGE_SIZE 0x1000ul
#define PAGE_SHIFT 12

#define BINARY_ROUND_UP(x, agn) (((x) + (agn)-1) & (~(agn - 1)))
#define BINARY_ROUND_DOWN(x, agn) ((x) & (~(agn - 1)))

#define PAGE_ROUND_UP(x) (((x) + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1)))
#define PAGE_ROUND_DOWN(x) ((x) & (~(PAGE_SIZE - 1)))

#define MAX_MEM_ORDER 11

#define MAX_KMEM_OBJECT_BLOCK_NAME_LEN 16
#define OBJECT_ALIGNMENT 0x8

// bootmem;

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

// kmem buddy system;

struct kmem_free_area_struct
{
    struct list_node free_page_list;
    uint8 *free_area_map;
    size_t free_area_map_size;
};

void kmem_page_manager_init(void);
void *kmem_alloc_pages(ulong order);
void kmem_free_page(void *addr);
void kmem_free_pages(void *addr, size_t page_count);

// kmem object management;

struct kmem_object_block_struct
{
    // at 0;
    struct list_node free_slots;
    // at sizeof(struct list_node);
    struct list_node allocated_slots;
    // stat;
    size_t used;
    size_t total;
    size_t object_size;
    size_t page_count;
    // next block;
    struct kmem_object_block_struct *nxt;
};

struct kmem_object_manager_struct
{
    char block_name[MAX_KMEM_OBJECT_BLOCK_NAME_LEN];
    size_t object_size;
    struct kmem_object_block_struct *blocks;
    struct kmem_object_manager_struct *nxt;
};

void *kmem_alloc(size_t size);
void kmem_free(void *addr);

void *kmem_object_alloc(struct kmem_object_manager_struct *mgr);
void kmem_object_free(void *vaddr);

void kmem_object_free_block(struct kmem_object_block_struct *block);
struct kmem_object_block_struct *kmem_object_create_block(ulong page_order, size_t object_size);

struct kmem_object_manager_struct *kmem_object_create_manager(size_t object_size, const char *name);
void kmem_object_init(void);

// kmem;

struct kmem_node_struct
{
    // info;
    ulong begin_ppn;
    size_t num_ppn;
    uint8 *mem_map;
    size_t mem_map_size;
    // page_manager;
    struct spinlock page_manager_lock;
    struct kmem_free_area_struct free_areas[MAX_MEM_ORDER];
    // object_manager;
    struct spinlock object_manager_lock;
    struct kmem_object_manager_struct object_manager;
};

extern struct kmem_node_struct kmem;
void kmem_init(void);

#endif