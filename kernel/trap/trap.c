// trap.c
#include <asm/csr.h>
#include <asm/hart.h>
#include <asm/sbi.h>
#include <asm/registers.h>
#include <vmem.h>
#include <trap.h>
#include <kimage_defs.h>
#include <console.h>
#include <proc.h>
#include <signal.h>

void ktrap_install_handler(void) { w_stvec((uint64)kernel_handler); }

void ktrap_handler(void)
{
    uint64 sepc = r_sepc();
    uint64 sstatus = r_sstatus();
    // called from supervisor;
    uint64 cause = r_scause();
    switch (cause)
    {
    case SCAUSE_STI:
        // timer trap;
        // schedule next interrupt;
        ktrap_schedule_timer(SCHEDULING_TIME_SPAN);
        if (current_hart()->running_proc != NULL && current_hart()->running_proc->state == PROC_RUNNING)
            scheduler_yield();
        break;
    default:
        printf("[trap]unknown cause: %p, stval: %p, sepc: %p\n", cause, r_stval(), sepc);
        panic("ktrap_handler\n");
        break;
    }
    w_sepc(sepc);
    w_sstatus(sstatus);
}

// jumping from user space (trapframe);
void utrap_handler(void)
{
    // reminder: interrupt is disabled;
    ktrap_install_handler();

    struct proc_struct *p = current_hart()->running_proc;
    p->trapframe->user_pc = r_sepc();

    uint64 cause = r_scause();
    switch (cause)
    {
    case SCAUSE_STI:
        // timer trap;
        // schedule next interrupt;
        ktrap_schedule_timer(SCHEDULING_TIME_SPAN);
        scheduler_yield();
        break;
    case SCAUSE_USER_ECALL:
        // syscall;
        if (proc_is_killed(p))
            proc_exit(-1);
        p->trapframe->user_pc += 4;
        enable_interrupt();
        syscall_handler();
        break;
    default:
        printf("[proc]user program #%d unexpected interrupted, scause: %p, sepc: %p, killed\n", p->pid, cause, r_sepc());
        proc_kill(p->pid);
        break;
    }

    if (proc_is_killed(p))
        proc_exit(-1);

    utrap_return();
}

void utrap_return(void)
{
    struct proc_struct *p = current_hart()->running_proc;
    // from now on, the interrupt is disabled;
    disable_interrupt();
    ulong uhdlr_addr = KERNEL_USER_HANDLER_VA - KERNEL_USER_HANDLER_PA_BEGIN + VA_USER_USER_HANDLER_BEGIN;
    w_stvec(uhdlr_addr);

    p->trapframe->kernel_satp = r_satp();
    p->trapframe->kernel_handler_addr = (uint64)utrap_handler;
    p->trapframe->kernel_hartid = r_tp();

    uint64 sstatus = r_sstatus();
    sstatus &= (~SSTATUS_SPP);
    sstatus |= SSTATUS_SPIE;
    w_sstatus(sstatus);

    w_sepc(p->trapframe->user_pc);

    // call user_handler.c:user_return
    ulong uret_addr = KERNEL_USER_RETURN_VA - KERNEL_USER_HANDLER_PA_BEGIN + VA_USER_USER_HANDLER_BEGIN;
    ((void (*)(uint64))uret_addr)(SATP(VMEM_MODE, PMA_VA2PA(p->pgtbl)));
}

void ktrap_schedule_timer(uint64 delta)
{
    uint64 nxt = r_time();
    nxt += delta;
    sbi_set_timer(nxt);
}
