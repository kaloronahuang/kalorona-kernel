// syscall.c
#include <proc.h>
#include <console.h>
#include <trap.h>
#include <utilities/string.h>

static uint64 (*syscall_handlers[])(void) = {
#ifdef KERNEL_DEBUG_MODE
    [SYSCALL_CODE_DEBUG_YELL] sys_debug_yell,
#endif
    [SYSCALL_CODE_EXIT] sys_exit,
    [SYSCALL_CODE_FORK] sys_fork,
    [SYSCALL_CODE_KILL] sys_kill,
    [SYSCALL_CODE_SLEEP] syscall_sleep,
    [SYSCALL_CODE_UPTIME] syscall_uptime,
    [SYSCALL_CODE_WAIT] syscall_wait,
    [SYSCALL_CODE_SBRK] syscall_sbrk,
    [SYSCALL_CODE_GETPID] syscall_getpid,
};

uint64 sys_debug_yell(void)
{
    struct proc_struct *p = current_hart()->running_proc;
    printf("[sys_debug]pid #%d yelling with trapframe: \n", p->pid);
    print_buffer((void *)p->trapframe, sizeof(struct trapframe_struct));
    return 0;
}

uint64 sys_exit(void)
{
    struct trapframe_struct *frame = current_hart()->running_proc->trapframe;
    proc_exit(frame->a1);
    return 0;
}

uint64 sys_fork(void) { return proc_fork(); }

uint64 sys_kill(void)
{
    struct trapframe_struct *frame = current_hart()->running_proc->trapframe;
    return proc_kill(frame->a1);
}

uint64 syscall_sleep(void)
{
    struct trapframe_struct *frame = current_hart()->running_proc->trapframe;
    uint64 ticks = frame->a1;
    spinlock_acquire(&tick_lock);
    uint64 start = tick;
    while (tick - start < ticks)
    {
        if (proc_is_killed(current_hart()->running_proc))
        {
            // goodbye;
            spinlock_release(&tick_lock);
            return -1;
        }
        proc_sleep(&tick, &tick_lock);
    }
    spinlock_release(&tick_lock);
    return 0;
}

uint64 syscall_uptime(void)
{
    uint64 ret = 0;
    spinlock_acquire(&tick_lock);
    ret = tick;
    spinlock_release(&tick_lock);
    return ret;
}

uint64 syscall_wait(void)
{
    spinlock_acquire(&(proc_manager.lock));
    for (;;)
    {
        struct proc_struct *cp = current_hart()->running_proc;
        bool children = 0;
        int ret_pid = -1;
        void *ret_addr = cp->trapframe->a1;
        for (struct proc_struct *p = proc_manager.proc_list.nxt_proc; p != NULL; p = p->nxt_proc)
            if (p->parent == cp)
            {
                children = true;
                if (p->state == PROC_ZOMBIE)
                {
                    ret_pid = p->pid;
                    // copy the exit_state;
                    if (ret_addr != NULL && vm_memcpyout(cp->pgtbl, ret_addr, &(p->exit_state), sizeof(p->exit_state)) != 0)
                    {
                        // sth wrong with the cpy;
                        spinlock_release(&(proc_manager.lock));
                        return -1;
                    }
                    // goes fine, reap the proc;
                    proc_detach(p);
                    proc_reap_detached(p);
                    spinlock_release(&(proc_manager.lock));
                    return ret_pid;
                }
            }
        if (children == 0 || proc_is_killed(cp))
        {
            spinlock_release(&(proc_manager.lock));
            return -1;
        }
        proc_sleep(cp, &(proc_manager.lock));
    }
}

uint64 syscall_sbrk(void)
{
    long delta;
    uint64 ret = 0;
    struct proc_struct *p = current_hart()->running_proc;
    memcpy(&delta, &(p->trapframe->a1), sizeof(delta));

    spinlock_acquire(&(proc_manager.lock));

    if (delta < 0 && ((ulong)p->heap_vaddr - VA_USER_BEGIN) < -delta)
    {
        spinlock_release(&(proc_manager.lock));
        return -1;
    }

    p->program_break += delta;
    if ((ulong)p->program_break > (ulong)p->heap_vaddr)
    {
        // expand;
        int page_order = 0;
        while ((ulong)p->program_break > (ulong)p->heap_vaddr + (PAGE_SIZE << page_order))
            page_order++;
        void *addr = kmem_alloc_pages(page_order);
        vm_mappages(p->pgtbl, p->heap_vaddr, PMA_VA2PA(addr), PAGE_SIZE << page_order, PTE_FLAG_R | PTE_FLAG_W | PTE_FLAG_U);
        p->heap_vaddr += (PAGE_SIZE << page_order);
    }
    else if ((ulong)p->program_break <= (ulong)(p->heap_vaddr - PAGE_SIZE))
    {
        // shrink;
        ulong new_end = PAGE_ROUND_UP(p->program_break);
        ulong current_end = PAGE_ROUND_DOWN(p->heap_vaddr);
        ulong cnt = (current_end - new_end) >> PAGE_SHIFT;
        vm_unmappages(p->pgtbl, new_end, cnt, true);
    }
    spinlock_release(&(proc_manager.lock));
}

uint64 syscall_getpid(void) { return current_hart()->running_proc->pid; }

void syscall_handler(void)
{
    struct trapframe_struct *frame = current_hart()->running_proc->trapframe;
    int call_typ = frame->a7;
    frame->a0 = syscall_handlers[call_typ]();
}
