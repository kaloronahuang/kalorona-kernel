
OBJS = \
	asm/sbi.o \
	boot/entry.o \
	boot/entrypoint.o \
	boot/setup_mmu.o \
	console/print.o \
	device/device.o \
	device/memory.o \
	memory/buddy.o \
	memory/bootmem.o \
	memory/kmem.o \
	proc/hart.o \
	proc/proc.o \
	proc/trap_handler.o \
	proc/trap.o \
	utilities/algorithm/qsort.o \
	utilities/algorithm/list.o \
	utilities/string.o \
	utilities/bytes.o \
	kimage_defs.o \
	spinlock.o \
	signal.o \
	vkernel_entry.o \
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

UNAME_S = $(shell uname -s)
READELF =
ifeq ($(UNAME_S),Linux)
	READELF = readelf
endif
ifeq ($(UNAME_S),Darwin)
	READELF = /usr/local/opt/binutils/bin/greadelf 
endif

%.ld: %.lds.S
	$(CC) $(INCLUDE_PARAMS) -E -P -x c $< > $@

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

build/kernel.elf: $(OBJS) kernel.ld
	mkdir build/
	$(LD) $(LD_FLAGS) -T kernel.ld -o build/kernel.elf $(OBJS)

scripts/boot.scr: scripts/boot.script
	mkimage -c none -A riscv -T script -d scripts/boot.script scripts/boot.scr

build/fs.img: build/kernel.elf scripts/boot.scr
	sudo sh scripts/mkfs.sh

kernel: build/fs.img

kernel-image-report: build/kernel.elf
	$(READELF) -a build/kernel.elf > build/report

clean:
	rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*/*.d */*.asm */*.sym *.ld \
	$(OBJS) \
	$(D_FILES) \
	scripts/boot.scr
	rm -rf build/

QEMU = qemu-system-riscv64
QEMU_MACHINE_FLAGS = -machine virt -m 1024M -nographic -smp 1

UBOOT_FLAGS = -bios $(UBOOT_DIR)/spl/u-boot-spl.bin -device loader,file=$(UBOOT_DIR)/u-boot.itb,addr=0x80200000
KERNEL_FS_FLAGS = -drive if=none,file=$(PWD)/build/fs.img,format=raw,id=x0 \
	-device virtio-blk-device,drive=x0

OPENSBI_FLAGS = -bios $(SBI_BUILD)
KERNEL_IMG_FLAGS = -kernel build/kernel.elf

GDB_PORT = 25501
QEMU_DEBUG_FLAGS = -gdb tcp::$(GDB_PORT) -S

qemu-sbi: build/kernel.elf
	$(QEMU) $(QEMU_MACHINE_FLAGS) $(OPENSBI_FLAGS) $(KERNEL_IMG_FLAGS)

qemu-uboot: build/fs.img
	$(QEMU) $(QEMU_MACHINE_FLAGS) $(UBOOT_FLAGS) $(KERNEL_FS_FLAGS)

qemu-dump-dtb:
	$(QEMU) $(QEMU_MACHINE_FLAGS) $(UBOOT_FLAGS) $(KERNEL_FS_FLAGS) -machine dumpdtb=qemu.dtb
	dtc -o qemu.dts -O dts -I dtb qemu.dtb

qemu-debug-sbi: build/kernel.elf
	echo "Now QEMU started with debugging port binded at $(GDB_PORT)"
	$(QEMU) $(QEMU_MACHINE_FLAGS) $(OPENSBI_FLAGS) $(KERNEL_IMG_FLAGS) $(QEMU_DEBUG_FLAGS)

qemu-debug-uboot: build/fs.img
	echo "Now QEMU started with debugging port binded at $(GDB_PORT)"
	$(QEMU) $(QEMU_MACHINE_FLAGS) $(UBOOT_FLAGS) $(KERNEL_FS_FLAGS) $(QEMU_DEBUG_FLAGS)