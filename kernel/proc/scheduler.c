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
        spinlock_acquire(&(proc_manager.lock));
        bool locked = true;
        for (struct proc_struct *p = proc_manager.proc_list.nxt_proc; p != NULL; p = p->nxt_proc)
        {
            if (p->state == PROC_RUNNABLE)
            {
                p->state = PROC_RUNNING;
                locked = false;
                spinlock_release(&(proc_manager.lock));

                this_hart->running_proc = p;

                // timer;
                // enable timer interrupt;
                proc_switch(&(this_hart->context), &(p->context));
                
                this_hart->running_proc = NULL;

                spinlock_acquire(&(proc_manager.lock));
                locked = true;
            }
        }
        if (locked)
            spinlock_release(&(proc_manager.lock));
    }
}

void scheduler_switch(void)
{
    struct proc_struct *p = current_hart()->running_proc;
    // user status;
    bool interrupt_status = current_hart()->trap_enabled;
    proc_switch(&(p->context), &(current_hart()->context));
    // recover;
    current_hart()->trap_enabled = interrupt_status;
}

void scheduler_yield(void)
{
    struct proc_struct *p = current_hart()->running_proc;
    spinlock_acquire(&(proc_manager.lock));
    p->state = PROC_RUNNABLE;
    spinlock_release(&(proc_manager.lock));
    scheduler_switch();
}
