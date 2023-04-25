// kmem.c
// kmem managaes all available physical memory pages through PMA in virtual memory mode;
#include <kmem.h>
#include <lock.h>
#include <signal.h>
#include <utilities/string.h>
#include <device/memory.h>
#include <kimage_defs.h>
#include <console.h>
#include <vmem.h>
#include <utilities/bytes.h>

struct kmem_node_struct kmem;
struct list_node uni_managers;

// universal manager sizes;
static size_t uni_manager_sizes[] = {131072, 65536, 32768, 16384, 8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 0};
static char *uni_manager_names[] = {"umgr-128k", "umgr-64k", "umgr-32k", "umgr-16k", "umgr-8k", "umgr-4k", "umgr-2k", "umgr-1k", "umgr-512", "umgr-256", "umgr-128", "umgr-64", "umgr-32", ""};

/* useful managers BEGIN */
struct kmem_object_manager_struct *list_node_object_manager;
/* useful managers END */

void *kmem_alloc(size_t size)
{
    void *ret_vaddr = NULL;
    // umanager are sorted by size;
    for (struct list_node *mgr_node = uni_managers.nxt; mgr_node != NULL && ret_vaddr == NULL; mgr_node = mgr_node->nxt)
    {
        struct kmem_object_manager_struct *mgr = list_access(*mgr_node, struct kmem_object_manager_struct);
        if (mgr->object_size >= size)
            ret_vaddr = kmem_object_alloc(mgr);
    }
    return ret_vaddr;
}

void kmem_free(void *vaddr) { kmem_object_free(vaddr); }

void kmem_init(void)
{
    // init the kmem info;
    // copy these from bootmem;
    printf("[kmem]initializing\n");
    kmem.begin_ppn = uma_node.begin_ppn;
    kmem.num_ppn = uma_node.num_ppn;
    kmem.mem_map = uma_node.mem_map;
    kmem.mem_map_size = uma_node.mem_map_size;
    // init buddy;
    kmem_page_manager_init();
    // init object management;
    kmem_object_init();
    // initialize default managers;
    // lower-level API allows custom object allocation through own manager;
    // kmem_alloc & kmem_free use the universal allocators;
    list_node_object_manager = kmem_object_create_manager(sizeof(struct list_node), "list_node");
    for (int i = 0; uni_manager_sizes[i]; i++)
    {
        struct list_node *node = (struct list_node *)kmem_object_alloc(list_node_object_manager);
        node->val = (void *)kmem_object_create_manager(uni_manager_sizes[i], uni_manager_names[i]);
        list_insert(&uni_managers, node);
    }
}
