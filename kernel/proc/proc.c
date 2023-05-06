// proc.c
#include <proc.h>
#include <console.h>
#include <utilities/string.h>
#include <asm/hart.h>
#include <signal.h>

struct proc_manager_struct proc_manager;

struct proc_struct *proc_create()
{
    struct proc_struct *p = (struct proc_struct *)kmem_object_alloc(proc_manager.proc_object_manager);
    if (p == NULL)
        return NULL;
    spinlock_init(&(p->lock), "proc_lock");

    uint new_pid = 0;
    spinlock_acquire(&(proc_manager.lock));
    new_pid = ++proc_manager.pid_counter;
    spinlock_release(&(proc_manager.lock));

    spinlock_acquire(&(p->lock));
    p->pid = new_pid;
    p->state = PROC_RUNNABLE;
    p->killed = 0;
    p->exit_state = 0;
    spinlock_release(&(p->lock));

    p->parent = proc_manager.root_proc;
    p->stack_addr = VA_USER_USER_HANDLER_BEGIN;
    p->heap_addr = VA_USER_BEGIN;
    p->pgtbl = vm_user_make_pagetable();
    p->trapframe = (struct trapframe_struct *)kmem_alloc_pages(0);
    memset(p->trapframe, 0, sizeof(struct trapframe_struct));
    vm_mappages(p->pgtbl, VA_USER_TRAPFRAME_BEGIN, PMA_VA2PA(p->trapframe), sizeof(struct trapframe_struct), PTE_FLAG_R | PTE_FLAG_W);
    memset(&(p->context), 0, sizeof(struct context_struct));
}

void proc_reap(struct proc_struct *p)
{
    // TODO: How to lock? How to disassemble children processes to the root process?
    kmem_free((void *)p);
}

void proc_extend_stack(struct proc_struct *p, int page_order)
{
    if (p == NULL)
        panic("proc_extend_stack - NULL proc");
    ulong extended_pages = kmem_alloc_pages(page_order);
    ulong size = 1ul << page_order;
    p->stack_addr -= size;
    vm_mappages(p->pgtbl, (ulong)p->stack_addr, PMA_VA2PA(extended_pages), size, PTE_FLAG_R | PTE_FLAG_W);
}

void proc_extend_heap(struct proc_struct *p, int page_order)
{
    if (p == NULL)
        panic("proc_extend_stack - NULL proc");
    ulong extended_pages = kmem_alloc_pages(page_order);
    ulong size = 1ul << page_order;
    vm_mappages(p->pgtbl, (ulong)p->heap_addr, PMA_VA2PA(extended_pages), size, PTE_FLAG_R | PTE_FLAG_W);
    p->heap_addr += size;
}

void proc_init(void)
{
    printf("[proc]initializing user space\n");
    spinlock_init(&(proc_manager.lock), "proc_manager_lock");
    proc_manager.proc_object_manager = kmem_object_create_manager(sizeof(struct proc_struct), "proc_struct");
}