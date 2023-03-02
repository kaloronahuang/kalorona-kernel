// console.h
#include <types.h>
#include <lock.h>

#ifndef CONSOLE

#define CONSOLE

struct console_lock_struct
{
    struct spinlock lock;
    int locking_enabled;
};

extern struct console_lock_struct console_lock;

void console_init();

void putchar(const char c);

int printf(const char *fmt, ...);

#endif