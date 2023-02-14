// lock.h
#include <types.h>

#ifndef LOCK

#define LOCK

struct spinlock {
    uint locked;
    char *description;
};

void init_spinlock(struct spinlock *lk);

#endif