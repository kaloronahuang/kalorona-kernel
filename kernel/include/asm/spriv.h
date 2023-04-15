// spriv.h
#ifndef ASM_SPRIV

#define ASM_SPRIV

#include <types.h>

static inline void sfence_vma()
{
    asm volatile("sfence.vma zero, zero");
}

#endif