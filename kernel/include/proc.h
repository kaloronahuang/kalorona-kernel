// proc.h
#ifndef PROC

#define PROC

#include <types.h>
#include <parameters.h>
#include <lock.h>
#include <vmem.h>
#include <asm/hart.h>

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

    /* Protected by Manager Lock - Begin */
    struct proc_struct *prv_proc;
    struct proc_struct *nxt_proc;
    /* Protected by Manager Lock - End */

    struct proc_struct *parent;

    void *stack_vaddr;
    void *heap_vaddr;
    pagetable_t pgtbl;
    struct trapframe_struct *trapframe;
    struct context_struct context;
};

struct proc_manager_struct
{
    struct spinlock lock;

    /* Protected by Local Lock - Begin */
    uint pid_counter;
    struct proc_struct proc_list;
    /* Protected by Local Lock - End */

    struct proc_struct *root_proc;
    struct kmem_object_manager_struct *proc_object_manager;
};

extern char vkernel_stack[];
extern struct proc_manager_struct proc_manager;

extern void proc_swtch(struct context_struct *old_frame, struct context_struct *new_frame);

void proc_insert(struct proc_struct *head, struct proc_struct *node);
void proc_detach(struct proc_struct *node);

void proc_link(struct proc_struct *p);
void proc_unlink(struct proc_struct *p);

struct proc_struct *proc_create();
void proc_reparent(struct proc_struct *p, struct proc_struct *new_parent);
void proc_reap(struct proc_struct *p);

void *proc_extend_stack(struct proc_struct *p, int page_order);
void *proc_extend_heap(struct proc_struct *p, int page_order);

void proc_init(void);

void scheduler(void);

#endif
