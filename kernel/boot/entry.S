# entry.S
# To create a stack for C-compiled program and handover control to C world.

# Useful in linking;
.section .entry
.global __entry
__entry:
    # Create the stack for C world;
    # There will be MAX_CPU stacks;
    # Load data to the right stack;
    # SBI offers hartid at a0, fdt_tree at a1;
    # Get a copy of hartid at a2;
    mv a2, a0
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