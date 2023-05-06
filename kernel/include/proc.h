// proc.h
#ifndef PROC

#define PROC

#include <types.h>
#include <parameters.h>
#include <lock.h>
#include <vmem.h>
#include <asm/hart.h>
#include <utilities/algorithm.h>

enum proc_state
{
    PROC_SLEEPING,
    PROC_RUNNABLE,
    PROC_RUNNING,
    PROC_STOPPED,
    PROC_ZOMBIE
};

struct proc_struct
{
    struct spinlock lock;

    /* Protected by Local Lock - Begin */
    uint pid;
    enum proc_state state;
    int killed;
    int exit_state;
    /* Protected by Local Lock - End */

    struct proc_struct *parent;

    void *stack_addr;
    void *heap_addr;
    pagetable_t pgtbl;
    struct trapframe_struct *trapframe;
    struct context_struct context;
};

struct proc_manager_struct
{
    struct spinlock lock;

    /* Protected by Local Lock - Begin */
    uint pid_counter;
    /* Protected by Local Lock - End */

    struct proc_struct *root_proc;

    struct kmem_object_manager_struct *proc_object_manager;
};

extern struct proc_manager_struct proc_manager;

struct proc_struct *proc_create();
void proc_reap(struct proc_struct *p);

void proc_extend_stack(struct proc_struct *p, int page_order);

void proc_init(void);

#endif
