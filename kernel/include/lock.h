// lock.h
#ifndef LOCK

#define LOCK

#include <types.h>

struct spinlock
{
    uint locked;
    char *description;
    struct hart_struct *hart;
};

void spinlock_init(struct spinlock *lk, char *description);

void spinlock_acquire(struct spinlock *lk);

void spinlock_release(struct spinlock *lk);

int is_holding_spinlock(struct spinlock *lk);

struct sleeplock
{
    uint locked;
    struct spinlock lk;
    char *description;
    int pid;
};

void sleeplock_init(struct sleeplock *lk, char *description);

void sleeplock_acquire(struct sleeplock *lk);

void sleeplock_release(struct sleeplock *lk);

int is_holding_sleeplock(struct sleeplock *lk);

#endif
