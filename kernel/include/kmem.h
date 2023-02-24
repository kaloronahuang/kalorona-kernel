// kmem.h
#include <asm/spriv.h>
#include <asm/scsr.h>
#include <types.h>
#include <parameters.h>

#ifndef KMEM

#define KMEM

extern char *kmem_head;

typedef uint64 pte_t;
typedef uint64 *pagetable_t;

struct kmem_freepage {
    struct kmem_freepage *nxt;
};

void kmem_init(void);
void *kmem_alloc(void);
void kmem_free(void *phy_addr);

// Page definitions;
#define PAGE_SIZE 0x1000
#define PAGE_SIZE_OFFSET 3
#define PAGE_OFFSET_SHIFT 12

#define PAGE_ROUND_UP(x) (((x) + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1)))
#define PAGE_ROUND_DOWN(x) ((x) & (~(PAGE_SIZE - 1)))

// PTE Flags;
#define PTE_FLAG_V (1ULL << 0)
#define PTE_FLAG_R (1ULL << 1)
#define PTE_FLAG_W (1ULL << 2)
#define PTE_FLAG_X (1ULL << 3)
#define PTE_FLAG_U (1ULL << 4)

#endif KMEM