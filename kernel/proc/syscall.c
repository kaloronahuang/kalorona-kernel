// syscall.c
#include <proc.h>

void syscall_handler(void)
{
    struct trapframe_struct *frame = current_hart()->running_proc->trapframe;
    int call_typ = frame->a7;
    switch (call_typ)
    {
    case SYSCALL_CODE_EXIT:
        proc_exit(frame->a0);
        break;

    default:
        break;
    }
}