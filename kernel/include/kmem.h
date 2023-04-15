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

#define AREA_ORDER 11
#define AREA_BLOCK_SIZE (1ul << (AREA_ORDER - 1 + PAGE_OFFSET_WIDTH))
#define LOWBIT(x) ((-(x)) & (x))

// object:
// (struct kmem_free_object) header
// ...content...
// size_t size;
struct kmem_free_object
{
    struct kmem_free_object *fa_node;
    struct kmem_free_object *ch_nodes[2];
    size_t object_size;
};

// object page chain;
struct kmem_object_page
{
    void *page_vaddr;
    struct kmem_object_page *prv;
    struct kmem_object_page *nxt;
};

// continuous pages;
struct kmem_free_area
{
    // size keyworded;
    struct kmem_free_area *size_fa_node;
    struct kmem_free_area *size_ch_nodes[2];

    // address keyworded;
    struct kmem_free_area *addr_fa_node;
    struct kmem_free_area *addr_ch_nodes[2];

    size_t area_size;
};

struct kmem_struct
{
    struct spinlock lock;
    struct kmem_free_area *size_root_area;
    struct kmem_free_area *addr_root_area;

    struct kmem_free_object *free_object_root;
    struct kmem_object_page *object_pagelist;
};

extern struct kmem_struct kmem;

void kmem_init(void);

// low level API for allocating pages;
void *kmem_alloc_pages(size_t page_count);
void kmem_free_pages(void *addr, size_t page_count);

// high level API;
void *kmem_alloc(size_t size);
void kmem_free(void *addr);

#endif