# vkernel_entry.s
# Prep for C-compiled vkernel to load its stack in virtual addressing space;

#include <parameters.h>

.section .text
.global __vkernel_entry
__vkernel_entry:
    # tp configured;
    # pass the a0, a1 as normal;
    # a2 is the offset of targeted stack;
    li a2, VKERNEL_STACK_SIZE
    la sp, vkernel_stack
    # a3 is the hart offset;
    mv a3, tp
    addi a3, a3, 1
    mul a2, a2, a3
    add sp, sp, a2
    call kernel_main

spin:
    j spin
