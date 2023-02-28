// lock.h
#include <types.h>

#ifndef LOCK

#define LOCK

struct spinlock
{
    uint locked;
    char *description;
    struct hart *hart;
};

void spinlock_init(struct spinlock *lk, char *description);

void spinlock_acquire(struct spinlock *lk);

void spinlock_release(struct spinlock *lk);

int is_holding_spinlock(struct spinlock *lk);

#endif