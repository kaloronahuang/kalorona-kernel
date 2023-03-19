// kmem.c
#include <kmem.h>
#include <lock.h>
#include <signal.h>
#include <utilities/string.h>
#include <device/memory.h>

struct
{
    struct kmem_freepage *freepage_head;
    struct spinlock lock;
} kmem;

void kmem_init(void)
{
    spinlock_init(&kmem.lock, "kernel_mem_lock");
    for (int seg_id = 0; seg_id < ram_descriptor.available_ram_segments_num; seg_id++)
    {
        char *p = ram_descriptor.available_ram_segments[seg_id].pa_begin;
        size_t len = ram_descriptor.available_ram_segments[seg_id].pa_len;
        while (len)
            kmem_free((void *)p), len -= PAGE_SIZE, p += PAGE_SIZE;
    }
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

void kmem_free(void *addr)
{
    struct kmem_freepage *ptr;

    if (((((uint64)addr) & (PAGE_SIZE - 1)) != 0) /* TODO: more expressions to go */)
        panic("kmem_free");

    memset(addr, 0, PAGE_SIZE);

    ptr = (struct kmem_freepage *)addr;

    spinlock_acquire(&kmem.lock);
    ptr->nxt = kmem.freepage_head;
    kmem.freepage_head = ptr;
    spinlock_release(&kmem.lock);
}
