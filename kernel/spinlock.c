// spinlock.h
#include <types.h>
#include <lock.h>
#include <asm/hart.h>
#include <signal.h>

void spinlock_init(struct spinlock *lk, char *description)
{
    lk->description = description;
    lk->locked = 0;
    lk->hart = NULL;
}

void spinlock_acquire(struct spinlock *lk)
{
    pushoff_hart();

    if (is_holding_spinlock(lk))
        panic("spinlock_acquire - spinlock held;");

    while (__sync_lock_test_and_set(&lk->locked, 1) != 0)
        ;
    __sync_synchronize();

    lk->hart = current_hart();
}

void spinlock_release(struct spinlock *lk)
{
    if (!is_holding_spinlock(lk))
        panic("spinlock_release - spinlock not held;");
    lk->hart = 0;
    
    __sync_synchronize();

    __sync_lock_release(&lk->locked);
    popoff_hart();
}

int is_holding_spinlock(struct spinlock *lk)
{
    return (lk->hart == current_hart()) && (lk->locked);
}