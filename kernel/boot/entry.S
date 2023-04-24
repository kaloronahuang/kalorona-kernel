# entry.S
# To create a stack for C-compiled program and handover control to C world.

.section .boot.bss
.global vkernel_entry
vkernel_entry:
    .dword __vkernel_entry

.section .boot.text
.global __entry
__entry:
    # Handing over from U-Boot or other bootloaders;
    # tp register has been written;
    # a0, a1 - argc, argv
    # Set the size of the stack;
    li a2, 512
    mul a2, a2, tp
    addi a2, a2, 512
    # Set the stack pointer;
    la sp, init_stack
    # Stack pointer decreases gradually with the more data loaded into the stack;
    add sp, sp, a2
    # Go!
    call kernel_entrypoint
