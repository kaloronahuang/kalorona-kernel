// proc.h
#ifndef PROC

#define PROC

#include <types.h>
#include <parameters.h>
#include <lock.h>
#include <vmem.h>
#include <asm/hart.h>
#include <utilities/algorithm.h>

enum procstate
{
    UNUSED,
    USED,
    SLEEPING,
    RUNNABLE,
    RUNNING,
    ZOMBIE
};

struct proc_struct
{
    struct spinlock lock;

    int pid;
    enum procstate state;
    int killed;
    int exit_status;

    struct proc_struct *parent;

    void *stack_addr;
    size_t mem_siz;
    pagetable_t pgtbl;
    struct trapframe_struct *trapframe;
    struct context_struct context;
};

struct uspace
{
    /* list of struct proc_struct */
    struct list_node proc_list;

};

void proc_init(void);

#endif