// proc.h
#include <types.h>
#include <parameters.h>

#ifndef PROC

#define PROC

struct context
{
};

struct hart
{
    struct proc *running_proc;
    struct context cxt;
    int trap_pushoff_dep;
    int trap_enabled;
};

extern size_t hart_count;

extern struct hart harts[MAX_CPU];

uint current_hart_id(void);

struct hart *current_hart(void);

void pushoff_hart(void);

void popoff_hart(void);

struct proc
{
};

#endif