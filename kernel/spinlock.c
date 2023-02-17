// spinlock.h
#include <types.h>
#include <lock.h>
#include <proc.h>
#include <signal.h>

void init_spinlock(struct spinlock *lk, char *description)
{
    lk->description = description;
    lk->locked = 0;
    lk->hart = NULL;
}

void acquire_spinlock(struct spinlock *lk)
{
    pushoff_hart();

    if (is_holding_spinlock(lk))
        panic("acquire_spinlock - spinlock held;");

    while (__sync_lock_test_and_set(&lk->locked, 1) != 0)
        ;
    __sync_synchronize();

    lk->hart = current_hart();
}

void release_spinlock(struct spinlock *lk)
{
    if (!is_holding_spinlock(lk))
        panic("release_spinlock - spinlock not held;");
    lk->hart = 0;
    
    __sync_synchronize();

    __sync_lock_release(&lk->locked);
    popoff_hart();
}

int is_holding_spinlock(struct spinlock *lk)
{
    return (lk->hart == current_hart()) && (lk->locked);
}