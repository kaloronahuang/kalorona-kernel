// sleeplock.c
#include <lock.h>
#include <proc.h>

void sleeplock_init(struct sleeplock *lk, char *description)
{
    spinlock_init(&(lk->lk), "sleeplock_spinlock");
    lk->description = description;
    lk->locked = 0;
    lk->pid = 0;
}

void sleeplock_acquire(struct sleeplock *lk)
{
    spinlock_acquire(&(lk->lk));
    while (lk->locked)
        proc_sleep(lk, &(lk->lk));
    lk->locked = 1;
    lk->pid = current_hart()->running_proc->pid;
    spinlock_release(&(lk->lk));
}

void sleeplock_release(struct sleeplock *lk)
{
    spinlock_acquire(&(lk->lk));
    proc_wakeup(lk);
    lk->locked = 0;
    lk->pid = 0;
    spinlock_release(&(lk->lk));
}

int is_holding_sleeplock(struct sleeplock *lk)
{
    int res = 0;
    spinlock_acquire(&(lk->lk));
    res = (lk->locked && lk->pid == current_hart()->running_proc->pid);
    spinlock_release(&(lk->lk));
    return res;
}
