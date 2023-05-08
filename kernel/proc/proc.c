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

// create a new process;
// but not linked into the main chain, wait for user program image to load;
// - create user pagetable, stack, heap, trapframe and mapped them to pagetable;
// return pid;
int proc_create()
{
    struct proc_struct *p = (struct proc_struct *)kmem_object_alloc(proc_manager.proc_object_manager);
    if (p == NULL)
        return -1;

    spinlock_acquire(&(proc_manager.lock));

    uint new_pid = 0;
    new_pid = ++proc_manager.pid_counter;

    spinlock_release(&(proc_manager.lock));

    p->pid = new_pid;
    p->state = PROC_ALLOCATED;
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

    p->kstack_vaddr = kmem_alloc_pages(0);
    p->context.sp = (uint64)p->kstack_vaddr + (PAGE_SIZE << 0);
    p->context.ra = (uint64)utrap_return;

    proc_link(p);

    return p->pid;
}

struct proc_struct *proc_lock_and_find(int pid)
{
    struct proc_struct *p = NULL;
    spinlock_acquire(&(proc_manager.lock));
    for (struct proc_struct *i = proc_manager.proc_list.nxt_proc; i != NULL; i = i->nxt_proc)
        if (i->pid == pid)
        {
            p = i;
            break;
        }
    if (p == NULL)
    {
        spinlock_release(&(proc_manager.lock));
        return NULL;
    }
    return p;
}

void proc_reap(int pid)
{
    struct proc_struct *p = proc_lock_and_find(pid);
    if (p == NULL)
        return;
    // unlink;
    proc_detach(p);
    spinlock_release(&(proc_manager.lock));

    // clear the data;
    // trapframe;
    vm_unmappages(p->pgtbl, VA_USER_TRAPFRAME_BEGIN, sizeof(struct trapframe_struct), true);
    vm_unmap_user_handler(p->pgtbl);
    // kstack;
    kmem_free_pages(p->kstack_vaddr, 1);
    // stack;
    vm_unmappages(p->pgtbl, (ulong)p->stack_vaddr, (VA_USER_USER_HANDLER_BEGIN - (ulong)p->stack_vaddr) >> PAGE_SHIFT, true);
    // heap;
    vm_unmappages(p->pgtbl, (ulong)p->heap_vaddr, ((ulong)p->heap_vaddr - VA_USER_BEGIN) >> PAGE_SHIFT, true);
    // free the proc;
    kmem_free((void *)p);
}

void proc_set_state(int pid, enum proc_state state)
{
    struct proc_struct *p = proc_lock_and_find(pid);
    if (p == NULL)
        return;
    p->state = state;
    spinlock_release(&(proc_manager.lock));
}

void *proc_extend_stack(int pid, int page_order)
{
    struct proc_struct *p = proc_lock_and_find(pid);
    if (p == NULL)
        return NULL;
    ulong extended_pages = (ulong)kmem_alloc_pages(page_order);
    ulong size = (1ul << (page_order + PAGE_SHIFT));
    p->stack_vaddr -= size;
    vm_mappages(p->pgtbl, (ulong)p->stack_vaddr, PMA_VA2PA(extended_pages), size, PTE_FLAG_R | PTE_FLAG_W | PTE_FLAG_U);
    spinlock_release(&(proc_manager.lock));
    return (void *)extended_pages;
}

void *proc_extend_heap(int pid, int page_order)
{
    struct proc_struct *p = proc_lock_and_find(pid);
    if (p == NULL)
        return NULL;
    ulong extended_pages = (ulong)kmem_alloc_pages(page_order);
    ulong size = (1ul << (page_order + PAGE_SHIFT));
    vm_mappages(p->pgtbl, (ulong)p->heap_vaddr, PMA_VA2PA(extended_pages), size, PTE_FLAG_R | PTE_FLAG_W | PTE_FLAG_X | PTE_FLAG_U);
    p->heap_vaddr += size;
    spinlock_release(&(proc_manager.lock));
    return (void *)extended_pages;
}

int proc_is_killed(struct proc_struct *p)
{
    int killed = 0;
    spinlock_acquire(&(proc_manager.lock));
    killed = p->killed;
    spinlock_release(&(proc_manager.lock));
    return killed;
}

void proc_exit(int status_code)
{
    spinlock_acquire(&(proc_manager.lock));
    struct proc_struct *p = current_hart()->running_proc;

    // take care of children;
    for (struct proc_struct *e = proc_manager.proc_list.nxt_proc; e != NULL; e = e->nxt_proc)
        if (e->parent == p)
            // TODO: wakeup parent;
            e->parent = proc_manager.root_proc;

    p->state = PROC_ZOMBIE;
    p->exit_state = status_code;

    spinlock_release(&(proc_manager.lock));

    // wakeup parent;

    scheduler_switch();
    // no came back;
}

void proc_kill(int pid)
{
}

int proc_fork(void)
{
}

void proc_test()
{
    char user_prog[] = {0x13, 0x05, 0x10, 0x00, 0x93, 0x08, 0xC0, 0x03, 0x73, 0x00, 0x00, 0x00};
    int pid = proc_create();
    char *image = (char *)proc_extend_heap(pid, 0);
    proc_extend_stack(pid, 0);
    for (int i = 0; i < 12; i++)
        image[i] = user_prog[i];
    proc_set_state(pid, PROC_RUNNABLE);
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
