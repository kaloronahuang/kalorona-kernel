// registers.h
#include <types.h>

#ifndef REGISTERS

#define REGISTERS

static inline uint64 r_tp()
{
    uint64 x;
    asm volatile("mv %0, tp"
                 : "=r"(x));
    return x;
}

static inline void w_tp(uint64 x)
{
    asm volatile("mv tp, %0"
                 :
                 : "r"(x));
}

#endif