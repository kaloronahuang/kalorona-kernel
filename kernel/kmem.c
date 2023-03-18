// kmem.c
#include <kmem.h>
#include <lock.h>
#include <signal.h>
#include <utilities/string.h>

struct
{
    struct kmem_freepage *freepage_head;
    struct spinlock lock;
} kmem;

extern char KERNEL_END[];

char *kmem_head;

void kmem_init(void)
{
    spinlock_init(&kmem.lock, "kernel_mem_lock");
    /*
    kmem_head = (char *)(PAGE_ROUND_UP((uint64)(KERNEL_END)));
    for (char *ptr = kmem_head; ptr < kmem_head + KMEM_SIZE; ptr += PAGE_SIZE)
        kmem_free(ptr);
    */
}

void *kmem_alloc(void)
{
    struct kmem_freepage *ptr;

    spinlock_acquire(&kmem.lock);
    ptr = kmem.freepage_head;
    if (ptr)
        kmem.freepage_head = ptr->nxt;
    spinlock_release(&kmem.lock);

    if (ptr)
        memset(ptr, 0, PAGE_SIZE);
    return (void *)ptr;
}

void kmem_free(void *phy_addr)
{
    struct kmem_freepage *ptr;

    if (((char *)(phy_addr) >= (kmem_head + KMEM_SIZE)) || ((char *)(phy_addr) < kmem_head) || ((((uint64)phy_addr) & (PAGE_SIZE - 1)) != 0))
        panic("kmem_free");

    memset(phy_addr, 0, PAGE_SIZE);

    ptr = (struct kmem_freepage *)phy_addr;

    spinlock_acquire(&kmem.lock);
    ptr->nxt = kmem.freepage_head;
    kmem.freepage_head = ptr;
    spinlock_release(&kmem.lock);
}
