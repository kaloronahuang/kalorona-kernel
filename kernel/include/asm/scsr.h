// scsr.h
#include <types.h>

#ifndef SCSR

#define SCSR

// CSR Header;
// r_* - Read CSR;
// read_* - Read information presented in CSR;

// CSR - sstatus;
// Supervisor Status Register;

// Previous mode;
#define SSTATUS_SPP (1LL << 8)
// Whether supervisor interrupts were enabled prior to trapping into supervisor mode;
#define SSTATUS_SPIE (1LL << 5)
// Whether all interrupts are enabled in supervisor mode;
#define SSTATUS_SIE (1LL << 1)

static inline uint64 r_sstatus()
{
    uint64 x;
    asm volatile("csrr %0, sstatus"
                 : "=r"(x));
    return x;
}

static inline void w_sstatus(uint64 x)
{
    asm volatile("csrw sstatus, %0"
                 :
                 : "r"(x));
}

static inline int read_interrupt()
{
    return (r_sstatus() & SSTATUS_SIE);
}

static inline void enable_interrupt()
{
    w_sstatus(r_sstatus() | SSTATUS_SIE);
}

static inline void disable_interrupt()
{
    w_sstatus(r_sstatus() & (~(SSTATUS_SIE)));
}

// CSR - stvec;
// Supervisor Trap Vector Base Address Register;

static inline uint64 r_stvec()
{
    uint64 x;
    asm volatile("csrr %0, stvec"
                 : "=r"(x));
    return x;
}

static inline void w_stvec(uint64 x)
{
    asm volatile("csrw stvec, %0"
                 :
                 : "r"(x));
}

// CSR - sip;
// Supervisor interrupt-pending register;

static inline uint64 r_sip()
{
    uint64 x;
    asm volatile("csrr %0, sip"
                 : "=r"(x));
    return x;
}

static inline void w_sip(uint64 x)
{
    asm volatile("csrw sip, %0"
                 :
                 : "r"(x));
}

// CSR - sie;
// Supervisor interrupt-enable register;

static inline uint64 r_sie()
{
    uint64 x;
    asm volatile("csrr %0, sie"
                 : "=r"(x));
    return x;
}

static inline void w_sie(uint64 x)
{
    asm volatile("csrw sie, %0"
                 :
                 : "r"(x));
}

// CSR - scounteren;
// Counter-Enable Register;

static inline uint64 r_scounteren()
{
    uint64 x;
    asm volatile("csrr %0, scounteren"
                 : "=r"(x));
    return x;
}

static inline void w_scounteren(uint64 x)
{
    asm volatile("csrw scounteren, %0"
                 :
                 : "r"(x));
}

// CSR - sscratch;
// Supervisor Scratch Register;

static inline uint64 r_sscratch()
{
    uint64 x;
    asm volatile("csrr %0, sscratch"
                 : "=r"(x));
    return x;
}

static inline void w_sscratch(uint64 x)
{
    asm volatile("csrw sscratch, %0"
                 :
                 : "r"(x));
}

// CSR - sepc;
// Supervisor Exception Program Counter;

static inline uint64 r_sepc()
{
    uint64 x;
    asm volatile("csrr %0, sepc"
                 : "=r"(x));
    return x;
}

static inline void w_sepc(uint64 x)
{
    asm volatile("csrw sepc, %0"
                 :
                 : "r"(x));
}

// CSR - scause;
// Supervisor Cause Register;

static inline uint64 r_scause()
{
    uint64 x;
    asm volatile("csrr %0, scause"
                 : "=r"(x));
    return x;
}

static inline void w_scause(uint64 x)
{
    asm volatile("csrw scause, %0"
                 :
                 : "r"(x));
}

// CSR - stval;
// Supervisor Trap Value;

static inline uint64 r_stval()
{
    uint64 x;
    asm volatile("csrr %0, stval"
                 : "=r"(x));
    return x;
}

static inline void w_stval(uint64 x)
{
    asm volatile("csrw stval, %0"
                 :
                 : "r"(x));
}

// CSR - senvcfg
// Supervisor Environment Configuration Register;

static inline uint64 r_senvcfg()
{
    uint64 x;
    asm volatile("csrr %0, senvcfg"
                 : "=r"(x));
    return x;
}

static inline void w_senvcfg(uint64 x)
{
    asm volatile("csrw senvcfg, %0"
                 :
                 : "r"(x));
}

// CSR - sscratch;
// Supervisor Scratch Register;

#define SATP_MODE_NONE 0
#define SATP_MODE_SV39 8
#define SATP_MODE_SV48 9
#define SATP_MODE_SV57 10
#define SATP_MODE_SV64 11

static inline uint64 r_satp()
{
    uint64 x;
    asm volatile("csrr %0, satp"
                 : "=r"(x));
    return x;
}

static inline void w_satp(uint64 x)
{
    asm volatile("csrw satp, %0"
                 :
                 : "r"(x));
}

#endif