// kimage_defs.h
// Linker script provides definitions about the kernel image;
#include <types.h>

#ifndef KIMAGE_DEFS

#define KIMAGE_DEFS

#ifdef PKERNEL

extern char __PA_BEGIN[];

extern char __KERNEL_IMG_PA_BEGIN[];
extern char __MEMORY_PA_BEGIN[];

extern char __KERNEL_USER_HANDLER_PA_BEGIN[];

extern char __KERNEL_TEXT_PA_BEGIN[];
extern char __KERNEL_RODATA_PA_BEGIN[];
extern char __KERNEL_DATA_PA_BEGIN[];
extern char __KERNEL_BSS_PA_BEGIN[];

extern char __KERNEL_IMG_PA_END[];

extern char __KERNEL_IMG_VA_BEGIN[];
extern char __KERNEL_IMG_VA_END[];

#else

extern uint64 pkernel_hsm_entry;

extern uint64 PA_BEGIN;

extern uint64 KERNEL_IMG_PA_BEGIN;
extern uint64 MEMORY_PA_BEGIN;

extern uint64 KERNEL_USER_HANDLER_PA_BEGIN;

extern uint64 KERNEL_TEXT_PA_BEGIN;
extern uint64 KERNEL_RODATA_PA_BEGIN;
extern uint64 KERNEL_DATA_PA_BEGIN;
extern uint64 KERNEL_BSS_PA_BEGIN;

extern uint64 KERNEL_IMG_PA_END;

extern uint64 KERNEL_IMG_VA_BEGIN;
extern uint64 KERNEL_IMG_VA_END;

#endif

#endif
