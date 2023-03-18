// kimage_defs.h
// Linker script provides definitions about the kernel image;

#ifndef KIMAGE_DEFS

#define KIMAGE_DEFS

extern uint64 PA_BEGIN[];

extern uint64 MEMORY_BEGIN[];

extern uint64 KERNEL_IMG_PA_BEGIN;
extern uint64 KERNEL_IMG_PA_END;
extern uint64 KERNEL_IMG_VA_BEGIN;
extern uint64 KERNEL_IMG_VA_END;

extern uint64 BOOT_SECTION_PA_BEGIN;

extern uint64 KERNEL_TEXT_PA_BEGIN;
extern uint64 KERNEL_RODATA_PA_BEGIN;
extern uint64 KERNEL_DATA_PA_BEGIN;
extern uint64 KERNEL_BSS_PA_BEGIN;

#endif