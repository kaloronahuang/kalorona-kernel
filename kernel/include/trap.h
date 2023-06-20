// trap.h

#ifndef TRAP

#define TRAP

#include <lock.h>

extern struct spinlock tick_lock;
extern ulong tick;
void tick_init(void);
void tick_handler(void);

void ktrap_install_handler(void);
void ktrap_handler(void);

void utrap_handler(void);
void utrap_return(void);

extern void kernel_handler(void);

void ktrap_schedule_timer(uint64 delta);

#endif