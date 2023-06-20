// syscall.c
#include <proc.h>
#include <console.h>

static uint64 (*syscall_handlers[])(void) = {
    [SYSCALL_CODE_EXIT] sys_exit,
    [SYSCALL_CODE_FORK] sys_fork,
    [SYSCALL_CODE_KILL] sys_kill,
#ifdef KERNEL_DEBUG_MODE
    [SYSCALL_CODE_DEBUG_YELL] sys_debug_yell
#endif
};

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

uint64 sys_debug_yell(void)
{
    struct proc_struct *p = current_hart()->running_proc;
    printf("[sys_debug]pid #%d yelling with trapframe: \n", p->pid);
    print_buffer((void *)p->trapframe, sizeof(struct trapframe_struct));
    return 0;
}

void syscall_handler(void)
{
    struct trapframe_struct *frame = current_hart()->running_proc->trapframe;
    int call_typ = frame->a7;
    frame->a0 = syscall_handlers[call_typ]();
}
