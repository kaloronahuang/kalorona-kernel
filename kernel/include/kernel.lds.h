// kernel.lds.h

#ifndef KERNEL_LDS

#define KERNEL_LDS

#define ALIGN_EXP(x, exp) (((x) + (exp) - 1) & (~((exp) - 1)))
#define SECTION_PA(prev_sec, align) ALIGN_EXP(LOADADDR(prev_sec) + SIZEOF(prev_sec), align)

#endif