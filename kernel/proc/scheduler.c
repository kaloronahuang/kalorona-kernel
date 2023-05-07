// scheduler.c
#include <asm/hart.h>
#include <proc.h>

void scheduler(void)
{
    struct hart_struct *this_hart = current_hart();
    this_hart->running_proc = NULL;
    for (;;)
    {
        enable_interrupt();
        for (struct proc_struct *p = proc_manager.proc_list.nxt_proc; p != NULL; p = p->nxt_proc)
        {
            spinlock_acquire(&(p->lock));
            if (p->state == PROC_RUNNABLE)
            {
                p->state = PROC_RUNNING;
                this_hart->running_proc = p;
                proc_swtch(&(this_hart->context), &(p->context));
                this_hart->running_proc = NULL;
            }
            spinlock_release(&(p->lock));
        }
    }
}