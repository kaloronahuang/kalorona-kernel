
OBJS = \
	boot/entry.o \
	boot/entrypoint.o \
	asm/sbi.o \
	utilities/string.o \
	utilities/bytes.o \
	console/print.o \
	proc/hart.o \
	spinlock.o \
	signal.o \
	kmem.o \
	vmem.o \
	main.o

INCLUDE_DIR = include/

CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)gas
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

CFLAGS = -Wall -O0 -fno-omit-frame-pointer -ggdb -gdwarf-2
CFLAGS += -MD
CFLAGS += -ffreestanding -fno-common -nostdlib
CFLAGS += -I. -mno-relax -fno-pie -no-pie -mcmodel=medany

LD_FLAGS = -z max-page-size=4096

# Dependency: libfdt;
LIBFDT_DIR = device/libfdt
include $(LIBFDT_DIR)/Makefile.libfdt
INCLUDE_DIR += $(LIBFDT_DIR)
OBJS += $(addprefix $(LIBFDT_DIR)/, $(LIBFDT_OBJS))

# Sum up;

D_FILES = $(OBJS:%.o=%.d)
INCLUDE_PARAMS = $(foreach d, $(INCLUDE_DIR), -I$d)
CFLAGS += $(INCLUDE_PARAMS)

kernel.elf: $(OBJS) kernel.ld
	$(LD) $(LD_FLAGS) -T kernel.ld -o kernel.elf $(OBJS)
	readelf -a kernel.elf > report

kernel: kernel.elf

clean:
	rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*/*.d */*.asm */*.sym \
	kernel.elf \
	report \
	$(OBJS) \
	$(D_FILES)

QEMU = qemu-system-riscv64
GDB_PORT = 25501
QEMU_FLAGS = -machine virt -m 1024M -nographic -bios $(SBI_BUILD) -smp 1
QEMU_KERNEL_FLAG = -kernel kernel.elf
QEMU_DEBUG_FLAGS = -gdb tcp::$(GDB_PORT) -S

qemu: kernel.elf
	$(QEMU) $(QEMU_FLAGS) $(QEMU_KERNEL_FLAG)

qemu_dump_dtb:
	$(QEMU) $(QEMU_FLAGS) -machine dumpdtb=qemu.dtb
	dtc -o dump.dts -O dts -I dtb qemu.dtb

qemu_debug: kernel.elf
	echo "Now QEMU started with debugging port binded at $(GDB_PORT)"
	$(QEMU) $(QEMU_FLAGS) $(QEMU_KERNEL_FLAG) $(QEMU_DEBUG_FLAGS)
