// kmem.h
#include <asm/spriv.h>
#include <asm/scsr.h>
#include <types.h>
#include <parameters.h>

#ifndef KMEM

#define KMEM

extern char *kmem_head;

struct kmem_freepage {
    struct kmem_freepage *nxt;
};

void kmem_init(void);
void *kmem_alloc(void);
void kmem_free(void *phy_addr);

// Page definitions;
#define PAGE_SIZE 0x1000
#define PAGE_SIZE_OFFSET 3

#define PAGE_ROUND_UP(x) (((x) + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1)))
#define PAGE_ROUND_DOWN(x) ((x) & (~(PAGE_SIZE - 1)))

#endif KMEM