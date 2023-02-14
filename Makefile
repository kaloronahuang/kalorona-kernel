
FILES = \
	kernel/boot/entry \
	kernel/riscv/sbi \
	kernel/utilities/string \
	kernel/console/print \
	kernel/spinlock \
	kernel/entrypoint

OBJS = $(foreach d, $(FILES), $d.o)
MAKEFILE_D = $(foreach d, $(FILES), $d.d)

INCLUDE_DIR = kernel/include
INCLUDE_PARAMS = $(foreach d, $(INCLUDE_DIR), -I$d)

TOOLCHAIN_PREFIX = $(CROSS_COMPILE)

CC = $(TOOLCHAIN_PREFIX)gcc
AS = $(TOOLCHAIN_PREFIX)gas
LD = $(TOOLCHAIN_PREFIX)ld
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy
OBJDUMP = $(TOOLCHAIN_PREFIX)objdump

CFLAGS = -Wall -O0 -fno-omit-frame-pointer -ggdb -gdwarf-2
CFLAGS += -MD
CFLAGS += -ffreestanding -fno-common -nostdlib
CFLAGS += -I. -mno-relax -fno-pie -no-pie -mcmodel=medany
CFLAGS += $(INCLUDE_PARAMS)
# CFLAGS += -Werror

LD_FLAGS = -z max-page-size=4096

#%.o: %.c
#	$(CC) $(CFLAGS) -c -o $@ $<

kernel/kernel.elf: $(OBJS) kernel/kernel.ld
	$(LD) $(LD_FLAGS) -T kernel/kernel.ld -o kernel/kernel.elf $(OBJS)
	readelf -a kernel/kernel.elf > kernel/report

kernel: kernel/kernel.elf

clean:
	rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*/*.d */*.asm */*.sym \
	kernel/kernel.elf \
	kernel/report \
	$(OBJS) \
	$(MAKEFILE_D)

QEMU = qemu-system-riscv64
GDB_PORT = 25501
QEMU_FLAGS = -machine virt -m 128M -nographic -bios $(SBI_BUILD)
QEMU_KERNEL_FLAG = -kernel kernel/kernel.elf
QEMU_DEBUG_FLAGS = -gdb tcp::$(GDB_PORT) -S

qemu: kernel/kernel.elf
	$(QEMU) $(QEMU_FLAGS) $(QEMU_KERNEL_FLAG)

qemu-debug: kernel/kernel.elf
	echo "Now QEMU started with debugging port binded at $(GDB_PORT)"
	$(QEMU) $(QEMU_FLAGS) $(QEMU_KERNEL_FLAG) $(QEMU_DEBUG_FLAGS)

qemu-sbi:
	$(QEMU) $(QEMU_FLAGS)