// proc.c
#include <proc.h>
#include <console.h>
#include <utilities/string.h>
#include <asm/hart.h>
#include <signal.h>
#include <trap.h>

struct proc_manager_struct proc_manager;

static char user_prog[] = {0x39, 0x71, 0x6, 0xfc, 0x22, 0xf8, 0x80, 0x0, 0xaa, 0x87, 0x23, 0x26, 0xf4, 0xfc, 0x83, 0x27, 0xc4, 0xfc, 0x81, 0x27, 0x81, 0xcb, 0x83, 0x27, 0xc4, 0xfc, 0x1b, 0x87, 0x7, 0x0, 0x85, 0x47, 0x63, 0x14, 0xf7, 0x0, 0x85, 0x47, 0x49, 0xa0, 0x23, 0x26, 0x4, 0xfe, 0x23, 0x24, 0x4, 0xfe, 0x99, 0xa8, 0x83, 0x27, 0xc4, 0xfc, 0x1b, 0x97, 0x87, 0x1, 0x1b, 0x57, 0x87, 0x41, 0x83, 0x27, 0x84, 0xfe, 0x9b, 0x97, 0x87, 0x1, 0x9b, 0xd7, 0x87, 0x41, 0xb9, 0x8f, 0x9b, 0x97, 0x87, 0x1, 0x9b, 0xd7, 0x87, 0x41, 0x13, 0xf7, 0xf7, 0xf, 0x83, 0x27, 0x84, 0xfe, 0xc1, 0x17, 0xa2, 0x97, 0x23, 0x80, 0xe7, 0xfe, 0x83, 0x27, 0x84, 0xfe, 0xc1, 0x17, 0xa2, 0x97, 0x83, 0xc7, 0x7, 0xfe, 0x81, 0x27, 0x3, 0x27, 0xc4, 0xfe, 0xb9, 0x8f, 0x23, 0x26, 0xf4, 0xfe, 0x83, 0x27, 0x84, 0xfe, 0x85, 0x27, 0x23, 0x24, 0xf4, 0xfe, 0x83, 0x27, 0x84, 0xfe, 0x1b, 0x87, 0x7, 0x0, 0xcd, 0x47, 0xe3, 0xd1, 0xe7, 0xfa, 0x83, 0x27, 0xc4, 0xfc, 0xfd, 0x37, 0x81, 0x27, 0x3e, 0x85, 0x97, 0x0, 0x0, 0x0, 0xe7, 0x80, 0x20, 0xf6, 0xaa, 0x87, 0x3e, 0x85, 0xe2, 0x70, 0x42, 0x74, 0x21, 0x61, 0x82, 0x80, 0x1, 0x11, 0x6, 0xec, 0x22, 0xe8, 0x0, 0x10, 0xaa, 0x87, 0x23, 0x30, 0xb4, 0xfe, 0x23, 0x26, 0xf4, 0xfe, 0x11, 0x45, 0x97, 0x0, 0x0, 0x0, 0xe7, 0x80, 0xa0, 0xf3, 0x51, 0x45, 0x97, 0x0, 0x0, 0x0, 0xe7, 0x80, 0x0, 0xf3, 0x13, 0x5, 0x80, 0x2, 0x97, 0x0, 0x0, 0x0, 0xe7, 0x80, 0x40, 0xf2, 0x13, 0x5, 0x80, 0xc, 0x97, 0x0, 0x0, 0x0, 0xe7, 0x80, 0x80, 0xf1, 0x81, 0x47, 0x3e, 0x85, 0xe2, 0x60, 0x42, 0x64, 0x5, 0x61, 0x82, 0x80, 0x41, 0x11, 0x6, 0xe4, 0x22, 0xe0, 0x0, 0x8, 0x97, 0x0, 0x0, 0x0, 0xe7, 0x80, 0xe0, 0xfa, 0xaa, 0x87, 0x3e, 0x85, 0x97, 0x0, 0x0, 0x0, 0xe7, 0x80, 0x80, 0x1, 0x1, 0x0, 0xa2, 0x60, 0x2, 0x64, 0x41, 0x1, 0x82, 0x80, 0x73, 0x0, 0x0, 0x0, 0x82, 0x80, 0x85, 0x48, 0x73, 0x0, 0x0, 0x0, 0x82, 0x80, 0x89, 0x48, 0x73, 0x0, 0x0, 0x0, 0x82, 0x80, 0x8d, 0x48, 0x73, 0x0, 0x0, 0x0, 0x82, 0x80, 0x91, 0x48, 0x73, 0x0, 0x0, 0x0, 0x82, 0x80, 0x95, 0x48, 0x73, 0x0, 0x0, 0x0, 0x82, 0x80, 0x99, 0x48, 0x73, 0x0, 0x0, 0x0, 0x82, 0x80, 0x9d, 0x48, 0x73, 0x0, 0x0, 0x0, 0x82, 0x80, 0xa1, 0x48, 0x73, 0x0, 0x0, 0x0, 0x82, 0x80};

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

    p->kstack_vaddr = kmem_alloc_pages(0);

    p->trapframe = (struct trapframe_struct *)kmem_alloc_pages(0);
    memset(p->trapframe, 0, sizeof(struct trapframe_struct));
    p->trapframe->user_pc = VA_USER_BEGIN;
    p->trapframe->sp = (uint64)p->stack_vaddr;
    p->trapframe->kernel_sp = (uint64)p->kstack_vaddr + (PAGE_SIZE << 0);

    vm_mappages(p->pgtbl, VA_USER_TRAPFRAME_BEGIN, PMA_VA2PA(p->trapframe), sizeof(struct trapframe_struct), PTE_FLAG_R | PTE_FLAG_W);

    memset(&(p->context), 0, sizeof(struct context_struct));

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

void proc_reap_detached(struct proc_struct *p)
{
    // clear the data;
    // trapframe;
    vm_unmappages(p->pgtbl, VA_USER_TRAPFRAME_BEGIN, PAGE_ROUND_UP(sizeof(struct trapframe_struct)) >> PAGE_SHIFT, true);
    vm_unmap_user_handler(p->pgtbl);
    // kstack;
    kmem_free_pages(p->kstack_vaddr, 1);
    // stack;
    vm_unmappages(p->pgtbl, (ulong)p->stack_vaddr, (VA_USER_USER_HANDLER_BEGIN - (ulong)p->stack_vaddr) >> PAGE_SHIFT, true);
    // heap;
    vm_unmappages(p->pgtbl, VA_USER_BEGIN, ((ulong)p->heap_vaddr - VA_USER_BEGIN) >> PAGE_SHIFT, true);
    // free the proc;
    kmem_free((void *)p);
}

void proc_reap(int pid)
{
    struct proc_struct *p = proc_lock_and_find(pid);
    if (p == NULL)
        return;
    // unlink;
    proc_detach(p);
    spinlock_release(&(proc_manager.lock));

    proc_reap_detached(p);
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

void *proc_extend_heap(int pid, int page_order, ulong mem_flag)
{
    struct proc_struct *p = proc_lock_and_find(pid);
    if (p == NULL)
        return NULL;
    ulong extended_pages = (ulong)kmem_alloc_pages(page_order);
    ulong size = (1ul << (page_order + PAGE_SHIFT));
    vm_mappages(p->pgtbl, (ulong)p->heap_vaddr, PMA_VA2PA(extended_pages), size, mem_flag | PTE_FLAG_U);
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

void proc_sleep(void *chan, struct spinlock *lk)
{
    struct proc_struct *p = current_hart()->running_proc;

    if (lk != &(proc_manager.lock))
    {
        spinlock_acquire(&(proc_manager.lock));
        if (lk != NULL)
            spinlock_release(lk);
    }

    p->sleeping_chan = chan;
    p->state = PROC_SLEEPING;

    spinlock_release(&(proc_manager.lock));

    scheduler_switch();

    spinlock_acquire(&(proc_manager.lock));
    p->sleeping_chan = NULL;
    spinlock_release(&(proc_manager.lock));

    if (lk != NULL)
        spinlock_acquire(lk);
}

void proc_wakeup(void *chan)
{
    spinlock_acquire(&(proc_manager.lock));
    for (struct proc_struct *p = proc_manager.proc_list.nxt_proc; p != NULL; p = p->nxt_proc)
        if (p->sleeping_chan == chan && p->state == PROC_SLEEPING)
            p->state = PROC_RUNNABLE;
    spinlock_release(&(proc_manager.lock));
}

void proc_exit(int status_code)
{
    spinlock_acquire(&(proc_manager.lock));
    struct proc_struct *p = current_hart()->running_proc;

    // take care of children;
    for (struct proc_struct *e = proc_manager.proc_list.nxt_proc; e != NULL; e = e->nxt_proc)
        if (e->parent == p)
        {
            e->parent = proc_manager.root_proc;
            proc_wakeup(e->parent);
        }

    p->state = PROC_ZOMBIE;
    p->exit_state = status_code;

    spinlock_release(&(proc_manager.lock));

    // wakeup sleeping chan of parent;
    proc_wakeup(p->parent);

    scheduler_switch();
    // no came back;
}

int proc_kill(int pid)
{
    struct proc_struct *p = proc_lock_and_find(pid);
    if (p == NULL)
        return -1;
    p->killed = true;
    if (p->state == PROC_SLEEPING)
        p->state = PROC_RUNNABLE;
    spinlock_release(&(proc_manager.lock));
    return 0;
}

int proc_fork(void)
{
    struct proc_struct *p = current_hart()->running_proc;
    int dup_pid = proc_create();
    if (dup_pid == -1)
        return -1;
    struct proc_struct *dup = proc_lock_and_find(dup_pid);
    // copy all the way;
    dup->parent = p;
    dup->stack_vaddr = p->stack_vaddr;
    dup->heap_vaddr = p->heap_vaddr;
    ulong kernel_sp = dup->trapframe->kernel_sp;
    memcpy(dup->trapframe, p->trapframe, sizeof(struct trapframe_struct));
    dup->trapframe->a0 = 0;
    dup->trapframe->kernel_sp = kernel_sp;
    // the addressing space;
    vm_uvmcpy(p->pgtbl, dup->pgtbl, false);
    // something strange: not copying context;
    // reason: context here is actually the context at proc_switch;
    //         regs are already in trapframe;
    //         scheduler -> utrap_return -(trapframe data)-> user program;
    // set the process free;
    dup->state = PROC_RUNNABLE;
    spinlock_release(&(proc_manager.lock));
    return dup->pid;
}

void proc_spawn_test_prog()
{
    int pid = proc_create();
    char *image = (char *)proc_extend_heap(pid, 0, PTE_FLAG_R | PTE_FLAG_X);
    proc_extend_stack(pid, 0);
    for (int i = 0; i < sizeof(user_prog); i++)
        image[i] = user_prog[i];
    struct proc_struct *p = proc_lock_and_find(pid);
    p->trapframe->user_pc += 0xfc;
    spinlock_release(&(proc_manager.lock));
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
    proc_spawn_test_prog();
}
