// kimage_defs.h
// Linker script provides definitions about the kernel image;

#ifndef KIMAGE_DEFS

#define KIMAGE_DEFS

extern char PA_BEGIN[];

extern char MEMORY_BEGIN[];

extern char KERNEL_IMG_PA_BEGIN[];
extern char KERNEL_IMG_PA_END[];
extern char KERNEL_IMG_VA_BEGIN[];
extern char KERNEL_IMG_VA_END[];

extern char BOOT_SECTION_PA_BEGIN[];

extern char KERNEL_TEXT_PA_BEGIN[];
extern char KERNEL_RODATA_PA_BEGIN[];
extern char KERNEL_DATA_PA_BEGIN[];
extern char KERNEL_BSS_PA_BEGIN[];

#endif