// buddy.c
#include <kmem.h>
#include <lock.h>
#include <signal.h>
#include <utilities/string.h>
#include <device/memory.h>
#include <kimage_defs.h>
#include <console.h>
#include <vmem.h>
#include <utilities/bytes.h>

void kmem_page_manager_init(void)
{
    printf("[kmem_page_manager]initializing\n");
    spinlock_init(&kmem.page_manager_lock, "page_manager_lock");
    for (int ord = 0; ord < MAX_MEM_ORDER; ord++)
    {
        kmem.free_areas[ord].free_area_map_size = ((kmem.num_ppn >> (ord + 1)) + 8 - 1) >> 3;
        kmem.free_areas[ord].free_area_map = bootmem_alloc(kmem.free_areas[ord].free_area_map_size, sizeof(uint8), 0);
    }
    // free all unreserved pages;
    for (ulong pn = 0; pn < kmem.num_ppn; pn++)
        if (!test_bit(kmem.mem_map, pn))
        {
            test_and_set_bit(kmem.mem_map, pn);
            kmem_free_page((void *)PMA_PA2VA((pn + kmem.begin_ppn) << PAGE_SHIFT));
        }
}

void *kmem_alloc_pages(ulong order)
{
    if (order >= MAX_MEM_ORDER)
        panic("kmem_alloc_pages - too large");
    ulong current_order = order;
    spinlock_acquire(&kmem.page_manager_lock);
    void *ret = NULL;
    while (current_order < MAX_MEM_ORDER)
        if (kmem.free_areas[current_order].free_page_list.nxt != NULL)
        {
            // got a huge page of size 2^current_order;
            // return kmem_expand_block(kmem.free_areas[current_order].free_page_list.nxt, current_order, order);
            if (current_order < order)
                panic("kmem_alloc_pages - wrong direction");
            ulong page_vaddr = (ulong)(kmem.free_areas[current_order].free_page_list.nxt);
            for (ulong ord = current_order; ord > order; ord--)
            {
                ulong block_vaddr = page_vaddr;
                ulong block_pn = (PMA_VA2PA(block_vaddr) >> PAGE_SHIFT) - kmem.begin_ppn;

                ulong left_part = block_vaddr;
                ulong right_part = block_vaddr + ((1ul << (ord - 1)) << PAGE_SHIFT);

                // remove the large block;
                list_detach((struct list_node *)left_part);
                toggle_bit(kmem.free_areas[ord].free_area_map, block_pn >> (ord + 1));

                // left part splitted;
                list_insert(&kmem.free_areas[ord - 1].free_page_list, (struct list_node *)left_part);
                // right part splitted;
                list_insert(&kmem.free_areas[ord - 1].free_page_list, (struct list_node *)right_part);
            }
            // register;
            ulong page_pn = (PMA_VA2PA(page_vaddr) >> PAGE_SHIFT) - kmem.begin_ppn;
            list_detach((struct list_node *)page_vaddr);
            toggle_bit(kmem.free_areas[order].free_area_map, page_pn >> (order + 1));

            ret = (void *)page_vaddr;
            break;
        }
        else
            current_order++;
    spinlock_release(&kmem.page_manager_lock);
    return ret;
}

void kmem_free_page(void *addr)
{
    if ((ulong)addr & (PAGE_SIZE - 1))
        panic("kmem_free_page - bad address alignment");
    ulong pn = (PMA_VA2PA(addr) >> PAGE_SHIFT) - kmem.begin_ppn;
    spinlock_acquire(&kmem.page_manager_lock);
    if (!test_and_clear_bit(kmem.mem_map, pn))
        panic("kmem_free_page - freed twice");
    for (int dep = 0; dep < MAX_MEM_ORDER; dep++)
    {
        ulong current_pn = pn & (~((1ul << dep) - 1));
        ulong buddy_pn = current_pn ^ (1ul << dep);
        ulong map_pos = current_pn >> (dep + 1);
        if (!toggle_bit(kmem.free_areas[dep].free_area_map, map_pos) && dep != MAX_MEM_ORDER - 1)
        {
            // min(current_ppn, buddy_ppn) freed for size of 2^(dep + 1);
            // buddy was freed;
            // must detach the buddy node;
            struct list_node *buddy_node = (struct list_node *)(PMA_PA2VA((buddy_pn + kmem.begin_ppn) << PAGE_SHIFT));
            list_detach(buddy_node);
            continue;
        }
        else
        {
            // current_ppn freed for size of 2^dep;
            struct list_node *current_node = (struct list_node *)(PMA_PA2VA((current_pn + kmem.begin_ppn) << PAGE_SHIFT));
            // connecting;
            list_insert(&kmem.free_areas[dep].free_page_list, current_node);
            break;
        }
    }
    spinlock_release(&kmem.page_manager_lock);
}

void kmem_free_pages(void *addr, size_t page_count)
{
    if ((ulong)addr & (PAGE_SIZE - 1))
        panic("kmem_free_pages - bad address alignment");
    ulong vaddr = (ulong)addr;
    while (page_count--)
        kmem_free_page((void *)vaddr), vaddr += PAGE_SIZE;
}
