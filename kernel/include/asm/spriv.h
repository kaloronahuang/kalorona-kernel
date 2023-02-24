// spriv.h
#include <types.h>

#ifndef SPRIV

#define SPRIV

static inline void sfence_vma()
{
    asm volatile("sfence.vma zero, zero");
}

#endif