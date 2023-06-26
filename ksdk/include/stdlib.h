// stdlib.h

#ifndef STDLIB_H

#define STDLIB_H

#include <kernel/include/types.h>

#ifdef KSDK_DEBUG
void debug_yell();
#endif

void exit(int state_code);
int fork(void);
int kill(int pid);
int sleep(int ticks);
int uptime(void);
int wait(void *state_code);
int sbrk(long delta);
int getpid(void);

void *malloc(size_t size);
void free(void *addr);

#endif