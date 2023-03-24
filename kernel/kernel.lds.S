/*
    kernel.lds.S

    This is NOT a linker script. Building system will build one based on given config.

*/

#define __ASSEMBLER__

#include <vmem.h>
#include <kernel.lds.h>

OUTPUT_ARCH("riscv")
ENTRY(__entry)

SECTIONS
{
    . = 0x0;
    PROVIDE(__PA_BEGIN = .);

    . = 0x80000000;
    PROVIDE(__KERNEL_IMG_PA_BEGIN = .);
    PROVIDE(__MEMORY_PA_BEGIN = .);
    
    . += 2M;
    BOOT_SECTION

    PROVIDE(__KERNEL_TEXT_PA_BEGIN = .);

    /* Kernel Addressing Space */
    . = VA_KERNEL_IMG_BEGIN;
    PROVIDE(__KERNEL_IMG_VA_BEGIN = .);

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