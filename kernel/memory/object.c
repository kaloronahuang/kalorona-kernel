// object.c
#include <kmem.h>
#include <signal.h>
#include <console.h>
#include <utilities/string.h>

void kmem_object_header_detach(struct kmem_object_header_struct *node)
{
    if (node == NULL)
        panic("kmem_object_header_detach - NULL argument");
    if (node->prv != NULL)
        node->prv->nxt = node->nxt;
    if (node->nxt != NULL)
        node->nxt->prv = node->prv;
    node->nxt = node->prv = NULL;
}

void kmem_object_header_insert(struct kmem_object_header_struct *head, struct kmem_object_header_struct *node)
{
    if (head == NULL || node == NULL)
        panic("kmem_object_header_insert - NULL argument");
    node->prv = head;
    node->nxt = head->nxt;
    if (node->nxt != NULL)
        node->nxt->prv = node;
    head->nxt = node;
}

static ulong kmem_object_alloc_from_block(struct kmem_object_block_struct *blk)
{
    if (blk->free_slots.nxt == NULL)
        panic("kmem_object_alloc_from_block - no slots available");
    struct kmem_object_header_struct *node = blk->free_slots.nxt;
    kmem_object_header_detach(node);
    kmem_object_header_insert(&(blk->allocated_slots), node);
    blk->used++;
    return (ulong)node + sizeof(struct kmem_object_header_struct);
}

void *kmem_object_alloc(struct kmem_object_manager_struct *mgr)
{
    bool allocated = false;
    ulong ret_vaddr = NULL;
    size_t allocated_sum = 0;
    spinlock_acquire(&(mgr->lock));
    for (struct kmem_object_block_struct *blk = mgr->blocks; blk != NULL; blk = blk->nxt)
        if (blk->free_slots.nxt != NULL)
        {
            allocated = true;
            ret_vaddr = kmem_object_alloc_from_block(blk);
            break;
        }
        else
            allocated_sum += blk->used * (blk->object_size + sizeof(struct kmem_object_header_struct));
    if (!allocated)
    {
        // new block maybe;
        ulong nxt_ord = 0;
        while (nxt_ord + 1 < MAX_MEM_ORDER && BINARY_ROUND_DOWN((1ul << (nxt_ord + PAGE_SHIFT)) - sizeof(struct kmem_object_block_struct), OBJECT_ALIGNMENT) <= allocated_sum)
            nxt_ord++;
        struct kmem_object_block_struct *new_blk = NULL;
        while ((new_blk = kmem_object_create_block(mgr, nxt_ord, mgr->object_size)) == NULL && nxt_ord > 0)
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
            ret_vaddr = kmem_object_alloc_from_block(new_blk);
        }
    }
    spinlock_release(&(mgr->lock));
    return (void *)ret_vaddr;
}

void kmem_object_free(void *vaddr)
{
    struct kmem_object_header_struct *header = (struct kmem_object_header_struct *)((ulong)vaddr - sizeof(struct kmem_object_header_struct));
    struct kmem_object_manager_struct *mgr = header->parent_block->parent_manager;

    spinlock_acquire(&(mgr->lock));

    kmem_object_header_detach(header);
    kmem_object_header_insert(&(header->parent_block->free_slots), header);
    header->parent_block->used--;

    spinlock_release(&(mgr->lock));
}

void kmem_object_free_block(struct kmem_object_block_struct *block)
{
    if (block->allocated_slots.nxt != NULL)
        panic("kmem_object_free_block - dirty block");
    struct kmem_object_manager_struct *mgr = block->parent_manager;
    spinlock_acquire(&(mgr->lock));

    // disconnect from the manager;
    if (mgr->blocks == block)
        mgr->blocks = block->nxt;
    else
        for (struct kmem_object_block_struct *i = mgr->blocks; i != NULL; i = i->nxt)
            if (i->nxt == block)
            {
                i->nxt = block->nxt;
                break;
            }
    // reclaim the memory to buddy system;
    kmem_free_pages((void *)block, block->page_count);

    spinlock_release(&(mgr->lock));
}

// returning a block, but not attached;
struct kmem_object_block_struct *kmem_object_create_block(struct kmem_object_manager_struct *parent_manager, ulong page_order, size_t object_size)
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
    block->object_size = BINARY_ROUND_UP(object_size, OBJECT_ALIGNMENT);
    block->page_count = 1ul << page_order;
    block->nxt = NULL;
    block->parent_manager = parent_manager;

    // create objects;
    block->allocated_slots.parent_block = block;
    block->free_slots.parent_block = block;

    struct kmem_object_header_struct *previous = &(block->free_slots);

    ulong page_end = (ulong)block + (1ul << (page_order + PAGE_SHIFT));
    size_t unit_size = sizeof(struct kmem_object_header_struct) + block->object_size;

    for (ulong pos = BINARY_ROUND_UP((ulong)block + sizeof(struct kmem_object_block_struct), OBJECT_ALIGNMENT);
         pos + unit_size <= page_end;
         pos += unit_size)
    {
        struct kmem_object_header_struct *cur = (struct kmem_object_header_struct *)pos;
        previous->nxt = cur;
        cur->prv = previous;
        cur->nxt = NULL;
        cur->parent_block = block;
        block->total++;
        previous = cur;
    }

    return block;
}

struct kmem_object_manager_struct *kmem_object_create_manager(size_t object_size, const char *name)
{
    struct kmem_object_manager_struct *new_mgr = (struct kmem_object_manager_struct *)kmem_object_alloc(&kmem.object_manager);
    strncpy(new_mgr->block_name, name, KMEM_OBJECT_BLOCK_NAME_MAXLEN);
    new_mgr->object_size = BINARY_ROUND_UP(object_size, OBJECT_ALIGNMENT);
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
    printf("[kmem_object]initializing\n");
    strcpy(kmem.object_manager.block_name, "kmem_obj_mgr");
    kmem.object_manager.object_size = BINARY_ROUND_UP(sizeof(struct kmem_object_manager_struct), OBJECT_ALIGNMENT);
    spinlock_init(&kmem.object_manager_lock, "kmem_object_manager_lock");
}