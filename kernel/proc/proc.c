// proc.c
#include <proc.h>
#include <console.h>
#include <utilities/string.h>
#include <asm/hart.h>
#include <signal.h>
#include <trap.h>

struct proc_manager_struct proc_manager;

void proc_insert(struct proc_struct *head, struct proc_struct *node)
{
    if (head == NULL || node == NULL)
        panic("proc_insert - null parameters");
    node->prv_proc = head;
    node->nxt_proc = head->nxt_proc;
    if (node->nxt_proc != NULL)
        node->nxt_proc->prv_proc = node;
    head->nxt_proc = node;
}

void proc_detach(struct proc_struct *node)
{
    if (node == NULL)
        panic("proc_detach - null parameters");
    if (node->prv_proc != NULL)
        node->prv_proc->nxt_proc = node->nxt_proc;
    if (node->nxt_proc != NULL)
        node->nxt_proc->prv_proc = node->prv_proc;
    node->prv_proc = node->nxt_proc = NULL;
}

void proc_link(struct proc_struct *p)
{
    spinlock_acquire(&(proc_manager.lock));
    proc_insert(&(proc_manager.proc_list), p);
    spinlock_release(&(proc_manager.lock));
}

void proc_unlink(struct proc_struct *p)
{
    spinlock_acquire(&(proc_manager.lock));
    proc_detach(p);
    spinlock_release(&(proc_manager.lock));
}

void proc_debut_return()
{
    spinlock_release(&(current_hart()->running_proc->lock));
    utrap_return();
}

// create a new process;
// but not linked into the main chain, wait for user program image to load;
// - create user pagetable, stack, heap, trapframe and mapped them to pagetable;
struct proc_struct *proc_create()
{
    struct proc_struct *p = (struct proc_struct *)kmem_object_alloc(proc_manager.proc_object_manager);
    if (p == NULL)
        return NULL;

    spinlock_acquire(&(proc_manager.lock));

    uint new_pid = 0;
    new_pid = ++proc_manager.pid_counter;

    spinlock_release(&(proc_manager.lock));

    p->pid = new_pid;
    p->state = PROC_RUNNABLE;
    p->killed = 0;
    p->exit_state = 0;

    p->parent = proc_manager.root_proc;
    p->stack_vaddr = (void *)VA_USER_USER_HANDLER_BEGIN;
    p->heap_vaddr = (void *)VA_USER_BEGIN;
    p->pgtbl = vm_user_make_pagetable();

    p->trapframe = (struct trapframe_struct *)kmem_alloc_pages(0);
    memset(p->trapframe, 0, sizeof(struct trapframe_struct));

    vm_mappages(p->pgtbl, VA_USER_TRAPFRAME_BEGIN, PMA_VA2PA(p->trapframe), sizeof(struct trapframe_struct), PTE_FLAG_R | PTE_FLAG_W);
    
    memset(&(p->context), 0, sizeof(struct context_struct));

    // debut return still in kernel mode;
    p->context.sp = (uint64)(vkernel_stack + (current_hart_id() + 1) * VKERNEL_STACK_SIZE);
    p->context.ra = (uint64)proc_debut_return;

    spinlock_init(&(p->lock), "proc_lock");

    return p;
}

void proc_reparent(struct proc_struct *p, struct proc_struct *new_parent)
{
    spinlock_acquire(&(p->lock));
    p->parent = new_parent;
    spinlock_release(&(p->lock));
}

// after the process exit/killed, call proc_reap;
void proc_reap(struct proc_struct *p)
{
    spinlock_acquire(&(proc_manager.lock));
    // reparent;
    for (struct proc_struct *e = proc_manager.proc_list.nxt_proc; e != NULL; e = e->nxt_proc)
        proc_reparent(e, proc_manager.root_proc);
    // unlink;
    proc_detach(p);
    spinlock_release(&(proc_manager.lock));

    // clear the data;
    // trapframe;
    vm_unmappages(p->pgtbl, VA_USER_TRAPFRAME_BEGIN, sizeof(struct trapframe_struct), true);
    vm_unmap_user_handler(p->pgtbl);
    // stack;
    vm_unmappages(p->pgtbl, (ulong)p->stack_vaddr, (VA_USER_USER_HANDLER_BEGIN - (ulong)p->stack_vaddr) >> PAGE_SHIFT, true);
    // heap;
    vm_unmappages(p->pgtbl, (ulong)p->heap_vaddr, ((ulong)p->heap_vaddr - VA_USER_BEGIN) >> PAGE_SHIFT, true);
    // free the proc;
    kmem_free((void *)p);
}

void *proc_extend_stack(struct proc_struct *p, int page_order)
{
    if (p == NULL)
        panic("proc_extend_stack - NULL proc");
    ulong extended_pages = (ulong)kmem_alloc_pages(page_order);
    ulong size = (1ul << (page_order + PAGE_SHIFT));
    p->stack_vaddr -= size;
    vm_mappages(p->pgtbl, (ulong)p->stack_vaddr, PMA_VA2PA(extended_pages), size, PTE_FLAG_R | PTE_FLAG_W);
    return (void *)extended_pages;
}

void *proc_extend_heap(struct proc_struct *p, int page_order)
{
    if (p == NULL)
        panic("proc_extend_heap - NULL proc");
    ulong extended_pages = (ulong)kmem_alloc_pages(page_order);
    ulong size = (1ul << (page_order + PAGE_SHIFT));
    vm_mappages(p->pgtbl, (ulong)p->heap_vaddr, PMA_VA2PA(extended_pages), size, PTE_FLAG_R | PTE_FLAG_W);
    p->heap_vaddr += size;
    return (void *)extended_pages;
}

void proc_test()
{
    char user_prog[] = {0x73, 0x00, 0x00, 0x00};
    struct proc_struct *test_prog = proc_create();
    char *image = (char *)proc_extend_heap(test_prog, 0);
    proc_extend_stack(test_prog, 0);
    for (int i = 0; i < 4; i++)
        image[i] = user_prog[i];
    test_prog->trapframe->user_pc = VA_USER_BEGIN;
    proc_link(test_prog);
}

void proc_init(void)
{
    printf("[proc]initializing user space\n");
    spinlock_init(&(proc_manager.lock), "proc_mgr_lock");
    proc_manager.proc_object_manager = kmem_object_create_manager(sizeof(struct proc_struct), "proc_struct");
    proc_manager.proc_list.nxt_proc = NULL;
    proc_manager.proc_list.prv_proc = NULL;
    // test purpose;
    proc_test();
}
