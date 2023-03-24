// kernel.lds.h

#ifndef KERNEL_LDS

#define KERNEL_LDS

#define ALIGN_EXP(x, exp) (((x) + (exp) - 1) & (~((exp) - 1)))
#define SECTION_PA(prev_sec, align) ALIGN_EXP(LOADADDR(prev_sec) + SIZEOF(prev_sec), align)

#define BOOT_SECTION \
    .boot.text : { \
        *(.boot.text .boot.text.*) \
    } \
    . = ALIGN(0x1000); \
    .boot.rodata : { \
        *(.boot.rodata .boot.rodata.*) \
    } \
    . = ALIGN(0x1000); \
    .boot.data : { \
        *(.boot.data .boot.data.*) \
    } \
    . = ALIGN(0x1000); \
    .boot.bss : { \
        *(.boot.bss .boot.bss.*) \
    } \
    . = ALIGN(0x1000);

#endif