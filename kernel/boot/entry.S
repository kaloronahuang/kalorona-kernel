# entry.S
# To create a stack for C-compiled program and handover control to C world.

# Useful in linking;
.section .text

.global __entry
__entry:
    # Handing over from U-Boot or other bootloaders;
    # tp register has been written, use it to calculate stack position;
    # a0, a1 - argc, argv
    mv a2, tp
    # Set the size of the stack for each hart;
    li a3, 4096
    # Set the stack pointer;
    la sp, init_stack
    # Stack pointer decreases gradually with the more data loaded into the stack;
    # Calculate the right address for each hart;
    addi a2, a2, 1
    mul a2, a3, a2
    add sp, sp, a2
    # Go!
    call kernel_entrypoint

# Doing nothing;
spin:
    j spin