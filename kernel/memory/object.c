// object.c
#include <kmem.h>
#include <signal.h>
#include <utilities/string.h>

void *kmem_object_alloc(struct kmem_object_manager_struct *mgr)
{
    bool allocated = false;
    ulong ret_vaddr = NULL;
    size_t allocated_sum = 0;
    spinlock_acquire(&kmem.object_manager_lock);
    for (struct kmem_object_block_struct *blk = mgr->blocks; blk != NULL; blk = blk->nxt)
        if (blk->free_slots.nxt != NULL)
        {
            allocated = true;
            struct list_node *node = blk->free_slots.nxt;
            list_detach(node);
            list_insert(&(blk->allocated_slots), node);
            blk->used++;
            ret_vaddr = (ulong)node + sizeof(struct list_node);
            break;
        }
        else
            allocated_sum += blk->used * blk->object_size;
    if (!allocated)
    {
        // new block maybe;
        ulong nxt_ord = 0;
        while (BINARY_ROUND_DOWN((1ul << (nxt_ord + PAGE_SHIFT)) - sizeof(struct kmem_object_block_struct), OBJECT_ALIGNMENT) <= allocated_sum)
            nxt_ord++;
        struct kmem_object_block_struct *new_blk = NULL;
        while ((new_blk = kmem_object_create_block(nxt_ord, mgr->object_size)) == NULL && nxt_ord > 0)
            nxt_ord--;
        if (new_blk == NULL)
            ret_vaddr = NULL;
        else
        {
            // connect the block to the manager;
            new_blk->nxt = mgr->blocks;
            mgr->blocks = new_blk;
            // allocate the object;
            allocated = true;
            struct list_node *node = new_blk->free_slots.nxt;
            list_detach(node);
            list_insert(&(new_blk->allocated_slots), node);
            new_blk->used++;
            ret_vaddr = (ulong)node + sizeof(struct list_node);
        }
    }
    spinlock_release(&kmem.object_manager_lock);
    return (void *)ret_vaddr;
}

void kmem_object_free(void *vaddr)
{
    spinlock_acquire(&kmem.object_manager_lock);

    struct list_node *connection = (struct list_node *)((ulong)vaddr - sizeof(struct list_node));
    struct list_node *node = connection->prv;
    list_detach(connection);

    while (node->prv != NULL)
        node = node->prv;
    // node is the list_node allocated in struct kmem_object_block_struct;
    struct kmem_object_block_struct *obj_block = (struct kmem_object_block_struct *)((ulong)node - sizeof(struct list_node));
    list_insert(&obj_block->free_slots, connection);
    obj_block->used--;

    spinlock_release(&kmem.object_manager_lock);
}

void kmem_object_free_block(struct kmem_object_block_struct *block) { kmem_free_pages((void *)block, block->page_count); }

struct kmem_object_block_struct *kmem_object_create_block(ulong page_order, size_t object_size)
{
    if (page_order >= MAX_MEM_ORDER)
        panic("kmem_object_create_block - invalid page_order");
    struct kmem_object_block_struct *block = kmem_alloc_pages(page_order);
    if (block == NULL)
        return NULL;
    // init the block;
    block->free_slots.nxt = block->allocated_slots.nxt = NULL;
    block->free_slots.prv = block->allocated_slots.prv = NULL;
    block->used = block->total = 0;
    block->object_size = object_size;
    block->page_count = 1ul << page_order;
    block->nxt = NULL;
    // create objects;
    struct list_node *previous = &(block->free_slots);
    ulong page_end = (ulong)block + (1ul << (page_order + PAGE_SHIFT));
    for (ulong pos = BINARY_ROUND_UP((ulong)block + sizeof(struct kmem_object_block_struct), OBJECT_ALIGNMENT);
         pos < page_end;
         pos += sizeof(struct list_node) + object_size)
    {
        struct list_node *cur = (struct list_node *)pos;
        previous->nxt = cur;
        cur->prv = previous;
        cur->nxt = NULL;
        block->total++;
    }
    return block;
}

struct kmem_object_manager_struct *kmem_object_create_manager(size_t object_size, const char *name)
{
    struct kmem_object_manager_struct *new_mgr = kmem_object_alloc(&kmem.object_manager);

    strncpy(new_mgr->block_name, name, MAX_KMEM_OBJECT_BLOCK_NAME_LEN);
    new_mgr->object_size = object_size;
    new_mgr->blocks = NULL;
    new_mgr->nxt = NULL;

    // link up to kmem;
    spinlock_acquire(&kmem.object_manager_lock);
    new_mgr->nxt = kmem.object_manager.nxt;
    kmem.object_manager.nxt = new_mgr;
    spinlock_release(&kmem.object_manager_lock);

    return new_mgr;
}

void kmem_object_init(void)
{
    printf("[kmem_object_manager]initializing\n");
    spinlock_init(&kmem.object_manager_lock, "object_manager_lock");
}