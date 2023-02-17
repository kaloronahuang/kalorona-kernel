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

void init_spinlock(struct spinlock *lk, char *description);

void acquire_spinlock(struct spinlock *lk);

void release_spinlock(struct spinlock *lk);

int is_holding_spinlock(struct spinlock *lk);

#endif