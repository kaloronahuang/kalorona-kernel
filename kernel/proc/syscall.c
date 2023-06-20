// syscall.c
#include <proc.h>
#include <console.h>
#include <trap.h>

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
    
}

uint64 syscall_sbrk(void)
{

}

uint64 syscall_getpid(void) { return current_hart()->running_proc->pid; }

void syscall_handler(void)
{
    struct trapframe_struct *frame = current_hart()->running_proc->trapframe;
    int call_typ = frame->a7;
    frame->a0 = syscall_handlers[call_typ]();
}
