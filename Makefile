
OBJS = \
	kernel/entry.o \
	kernel/entrypoint.o

TOOLCHAIN_PREFIX = riscv64-unknown-elf-

CC = $(TOOLCHAIN_PREFIX)gcc
AS = $(TOOLCHAIN_PREFIX)gas
LD = $(TOOLCHAIN_PREFIX)ld
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy
OBJDUMP = $(TOOLCHAIN_PREFIX)objdump

CC_FLAGS = -Wall -Werror -O0 -ggdb -gdwarf-2
CC_FLAGS += -nostartfiles -nostdlib -nodefaultlibs
CC_FLAGS += -I. -ffreestanding -fno-stack-protector

LD_FLAGS = -z max-page-size=4096

kernel/kernel.bin: $(OBJS) kernel/kernel.ld
	$(LD) $(LD_FLAGS) -T kernel/kernel.ld -o kernel/kernel.bin $(OBJS)

kernel: kernel/kernel.bin

clean:
	rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*/*.o */*.d */*.asm */*.sym \
	kernel/kernel.bin

QEMU = qemu-system-riscv64
GDB_PORT = 25501
QEMU_FLAGS = -machine virt -kernel kernel/kernel.bin -m 128M -nographic
QEMU_FLAGS += -bios $(SBI_BUILD)
QEMU_DEBUG_FLAGS = -gdb tcp::$(GDB_PORT) -S

qemu: kernel/kernel.bin
	$(QEMU) $(QEMU_FLAGS)

qemu-debug: kernel/kernel.bin
	echo "Now QEMU started with debugging port binded at $(GDB_PORT)"
	$(QEMU) $(QEMU_FLAGS) $(QEMU_DEBUG_FLAGS)