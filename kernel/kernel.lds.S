/*
    kernel.lds.S

    This is NOT a linker script. Building system will build one based on given config.

*/
#include <vmem_defs.h>

#define ALIGN_EXP(x, exp) (((x) + (exp) - 1) & (~((exp) - 1)))
#define SECTION_PA(prev_sec, align) ALIGN_EXP(LOADADDR(prev_sec) + SIZEOF(prev_sec), align)

OUTPUT_ARCH("riscv")
ENTRY(__entry)

SECTIONS
{
    . = 0x0;

    PROVIDE(__PA_BEGIN = .);
    PROVIDE(__KERNEL_IMG_PA_BEGIN = .);

    . = 0x80000000;

    PROVIDE(__MEMORY_BEGIN = .);
    
    . += 2M;
    PROVIDE(__BOOT_SECTION_PA_BEGIN = .);
    
    .boot.text : {
        boot/entry.o (.text)
        boot/entrypoint.o (.text)
    }

    .boot.rodata : {
        boot/entry.o (.rodata)
        boot/entrypoint.o (.rodata)
    }

    .boot.data : {
        boot/entry.o (.data)
        boot/entrypoint.o (.data)
    }

    .boot.bss : {
        boot/entry.o (.bss)
        boot/entrypoint.o (.bss)
    }

    . = ALIGN(0x1000);
    PROVIDE(__KERNEL_TEXT_PA_BEGIN = .);

    . = VMEM_KERNEL_SPACE_VA_BEGIN;
    PROVIDE(__KERNEL_IMG_VA_BEGIN = .);

    /*
        Virtual Memory Space;
        .boot lies here;
    */

    . += __KERNEL_TEXT_PA_BEGIN - __KERNEL_IMG_PA_BEGIN;

    .text : AT(__KERNEL_TEXT_PA_BEGIN) {
        *(.text .text.*)
    }
    . = ALIGN(0x1000);

    PROVIDE(__KERNEL_RODATA_PA_BEGIN = SECTION_PA(.text, 0x1000));
    .rodata : AT(SECTION_PA(.text, 0x1000)) {
        *(.srodata .srodata.*)
        . = ALIGN(16);
        *(.rodata .rodata.*)
        . = ALIGN(16);
    }
    . = ALIGN(0x1000);

    PROVIDE(__KERNEL_DATA_PA_BEGIN = SECTION_PA(.rodata, 0x1000));
    .data : AT(SECTION_PA(.rodata, 0x1000)) {
        *(.sdata .sdata.*)
        . = ALIGN(16);
        *(.data .data.*)
        . = ALIGN(16);
    }
    . = ALIGN(0x1000);

    PROVIDE(__KERNEL_BSS_PA_BEGIN = SECTION_PA(.data, 0x1000));
    .bss : AT(SECTION_PA(.data, 0x1000)) {
        *(.sbss .sbss.*)
        . = ALIGN(16);
        *(.bss .bss.*)
        . = ALIGN(16);
    }
    . = ALIGN(0x1000);

    PROVIDE(__KERNEL_IMG_VA_END = .);
    PROVIDE(__KERNEL_IMG_PA_END = SECTION_PA(.bss, 0x1000));
}