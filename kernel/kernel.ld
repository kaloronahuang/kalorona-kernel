/*
    kalorona-posix
    This is a file indicating the file organization;
*/

OUTPUT_ARCH( "riscv" )
ENTRY( __entry )


SECTIONS
{
    . = 0x80000000;

    .text : {
        *(.text .text.*)
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
}