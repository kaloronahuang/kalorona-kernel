/*
    kernel.ld
*/

OUTPUT_ARCH( "riscv" )
ENTRY( __entry )


SECTIONS
{
    /* Booting the kernel using OpenSBI now */
    . = 0x80200000;
    
    .entry : {
        *(.entry .entry.*)
        . = ALIGN(0x1000);
    }

    .text : {
        PROVIDE(KERNEL_TEXT_BEGIN = .);
        *(.text .text.*)
        . = ALIGN(0x1000);
        PROVIDE(KERNEL_TEXT_END = .);
    }

    .rodata : {
        . = ALIGN(16);
        *(.srodata .srodata.*)
        . = ALIGN(16);
        *(.rodata .rodata.*)
    }

    .data : {
        . = ALIGN(16);
        *(.sdata .sdata.*)
        . = ALIGN(16);
        *(.data .data.*)
    }

    .bss : {
        . = ALIGN(16);
        *(.sbss .sbss.*)
        . = ALIGN(16);
        *(.bss .bss.*)
    }

    PROVIDE(KERNEL_END = .);
}