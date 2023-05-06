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

struct kmem_object_header_struct
{
    struct kmem_object_block_struct *parent_block;
    struct kmem_object_header_struct *prv;
    struct kmem_object_header_struct *nxt;
};

struct kmem_object_block_struct
{
    /* Protected by Parent Manager Lock - Begin */
    struct kmem_object_header_struct free_slots;
    struct kmem_object_header_struct allocated_slots;
    struct kmem_object_block_struct *nxt;
    /* Protected by Parent Manager Lock - End */

    // stat;
    size_t used;
    size_t total;
    size_t object_size;
    size_t page_count;

    // parent manager;
    struct kmem_object_manager_struct *parent_manager;
};

void kmem_object_header_detach(struct kmem_object_header_struct *node);
void kmem_object_header_insert(struct kmem_object_header_struct *head, struct kmem_object_header_struct *node);

struct kmem_object_manager_struct
{
    char *block_name;
    size_t object_size;

    struct spinlock lock;
    /* Protected by Lock - Begin */
    struct kmem_object_block_struct *blocks;
    struct kmem_object_manager_struct *nxt;
    /* Protected by Lock - End */
};

void *kmem_alloc(size_t size);
void kmem_free(void *vaddr);

void *kmem_object_alloc(struct kmem_object_manager_struct *mgr);
void kmem_object_free(void *vaddr);

void kmem_object_free_block(struct kmem_object_block_struct *block);
struct kmem_object_block_struct *kmem_object_create_block(struct kmem_object_manager_struct *parent_manager, ulong page_order, size_t object_size);

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
    // The lock is only responsible for adding object_manager;
    struct spinlock object_manager_lock;
    struct kmem_object_manager_struct object_manager;
};

extern struct kmem_node_struct kmem;
void kmem_init(void);

// useful managers;

extern struct kmem_object_manager_struct *list_node_object_manager;

#endif