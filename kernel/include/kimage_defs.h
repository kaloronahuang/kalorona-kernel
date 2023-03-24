// kimage_defs.h
// Linker script provides definitions about the kernel image;
#include <types.h>

#ifndef KIMAGE_DEFS

#define KIMAGE_DEFS

#ifdef PKERNEL

extern char __PA_BEGIN[];

extern char __KERNEL_IMG_PA_BEGIN[];
extern char __MEMORY_PA_BEGIN[];

extern char __KERNEL_TEXT_PA_BEGIN[];
extern char __KERNEL_RODATA_PA_BEGIN[];
extern char __KERNEL_DATA_PA_BEGIN[];
extern char __KERNEL_BSS_PA_BEGIN[];

extern char __KERNEL_IMG_PA_END[];

extern char __KERNEL_IMG_VA_BEGIN[];
extern char __KERNEL_IMG_VA_END[];

#else

#ifndef __ASSEMBLER__

extern char PA_BEGIN[];

extern char KERNEL_IMG_PA_BEGIN[];
extern char MEMORY_PA_BEGIN[];

extern char KERNEL_TEXT_PA_BEGIN[];
extern char KERNEL_RODATA_PA_BEGIN[];
extern char KERNEL_DATA_PA_BEGIN[];
extern char KERNEL_BSS_PA_BEGIN[];

extern char KERNEL_IMG_PA_END[];

extern char KERNEL_IMG_VA_BEGIN[];
extern char KERNEL_IMG_VA_END[];

#else

#define KIMAGE_SYMOL(name) \
    .global name \
    name: \
        .dword __##name

#endif

#endif

#endif