// console.h
#ifndef CONSOLE

#define CONSOLE

#include <types.h>
#include <lock.h>

struct console_struct
{
    struct spinlock lock;
    int locking_enabled;
    struct device_struct *kernel_stdout_dev;
    struct device_struct *kernel_stdin_dev;
};

extern struct console_struct console;

void console_init();

void putchar(const char c);

char getchar(void);

int printf(const char *fmt, ...);

void print_buffer(void *buf, size_t size);

#endif