// hart.h
#ifndef ASM_HART

#define ASM_HART

#include <types.h>
#include <parameters.h>
#include <lock.h>

struct context_struct
{
    uint64 ra;
    uint64 sp;

    // callee-saved
    uint64 s0;
    uint64 s1;
    uint64 s2;
    uint64 s3;
    uint64 s4;
    uint64 s5;
    uint64 s6;
    uint64 s7;
    uint64 s8;
    uint64 s9;
    uint64 s10;
    uint64 s11;
};

struct trapframe_struct
{
    uint64 kernel_satp;
    uint64 kernel_sp;
    uint64 kernel_handler_addr;
    uint64 user_pc;
    uint64 kernel_hartid;
    uint64 ra;
    uint64 sp;
    uint64 gp;
    uint64 tp;
    uint64 t0;
    uint64 t1;
    uint64 t2;
    uint64 s0;
    uint64 s1;
    uint64 a0;
    uint64 a1;
    uint64 a2;
    uint64 a3;
    uint64 a4;
    uint64 a5;
    uint64 a6;
    uint64 a7;
    uint64 s2;
    uint64 s3;
    uint64 s4;
    uint64 s5;
    uint64 s6;
    uint64 s7;
    uint64 s8;
    uint64 s9;
    uint64 s10;
    uint64 s11;
    uint64 t3;
    uint64 t4;
    uint64 t5;
    uint64 t6;
};

struct hart_struct
{
    struct proc_struct *running_proc;
    struct context_struct context;
    int trap_pushoff_dep;
    int trap_enabled;
};

extern size_t hart_count;

extern struct hart_struct harts[MAX_CPU];

uint current_hart_id(void);

struct hart_struct *current_hart(void);

void pushoff_hart(void);

void popoff_hart(void);

#endif