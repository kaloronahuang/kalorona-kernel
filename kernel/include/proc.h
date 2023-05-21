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
    PROC_ZOMBIE,
    PROC_ALLOCATED
};

struct proc_struct
{
    /* Protected by Manager Lock - Begin */
    uint pid;
    enum proc_state state;
    int killed;
    int exit_state;
    struct proc_struct *prv_proc;
    struct proc_struct *nxt_proc;
    /* Protected by Manager Lock - End */

    struct proc_struct *parent;

    void *kstack_vaddr;
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

extern void proc_switch(struct context_struct *old_frame, struct context_struct *new_frame);

void proc_insert(struct proc_struct *head, struct proc_struct *node);
void proc_detach(struct proc_struct *node);

void proc_link(struct proc_struct *p);
void proc_unlink(struct proc_struct *p);

int proc_create();
struct proc_struct *proc_lock_and_find(int pid);
void proc_reap(int pid);
void proc_set_state(int pid, enum proc_state state);
void *proc_extend_stack(int pid, int page_order);
void *proc_extend_heap(int pid, int page_order);
bool proc_is_killed(struct proc_struct *p);

// call from user;
void proc_exit(int status_code);
void proc_kill(int pid);
void proc_wait();
void proc_sleep();
void proc_wakeup();
int proc_fork(void);

void proc_init(void);

#define SCHEDULING_TIME_SPAN 1000000

void scheduler(void);
void scheduler_switch(void);
void scheduler_yield(void);

// system call definitions;

#define SYSCALL_CODE_EXIT 60

void syscall_handler(void);

#endif
