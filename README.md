# kalorona-kernel

A toy kernel on RISC-V platform.

## Features

### Implemented

- Paging & Virtual Memory
- Load the kernel at kernel address space
- Memory Discovery & Pages Management: Allocating valid pages(not reserved)

### Planned

- Virtio Driver for I/O
- Device Discovery
- User Space

## Build

### Prerequisite

To build, you need the [RISC-V GNU Toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain). Then, set the environment variable `CROSS_COMPILE` to the prefix of your toolchain name. For example, if the RISC-V GCC is named `riscv64-unknown-linux-gnu-gcc`, then set `CROSS_COMPILE` to `riscv64-unknown-linux-gnu-`.

Besides, you need these packages:

```shell
sudo apt install u-boot-tools build-essential gdb-multiarch qemu-system-misc device-tree-compiler
```

In order to run properly, you also need OpenSBI environment and U-Boot bootloader.

#### OpenSBI

Please refer to Section "Execution on QEMU RISC-V 64-bit - **No Payload Case**" of [this article](https://github.com/riscv-software-src/opensbi/blob/master/docs/platform/qemu_virt.md) to build the OpenSBI. Then, set environment variable `SBI_BUILD` to the OpenSBI binary `fw_jump.elf`. Typically, it is located at `${Your OpenSBI root folder}/build/platform/generic/firmware/fw_jump.elf`.

#### U-Boot

Please refer to Section "Running U-Boot SPL" of [this article](https://u-boot.readthedocs.io/en/latest/board/emulation/qemu-riscv.html#running-u-boot-spl) to build U-Boot. You may make a link of the previous OpenSBI build to the root folder of U-Boot. Follow the instructions for 64-bit RISC-V. Then, set environment variable `UBOOT_DIR` to the directory of the U-Boot repo.

### Build Kernel

If you want to build the ELF file only, build the target `build/kernel.elf`:

```shell
cd kernel
make build/kernel.elf
```

To build a bootable disk image, build the target `kernel`. This will ask for root privilege to attach the disk image to a loopback device and mount the loopback device to a local folder.

```shell
cd kernel
make kernel
```

## Run & Debugging

It is recommended to use a bootloader(e.g U-Boot) to load the kernel ELF file. To run the kernel automatically:

```shell
make qemu-uboot
```

To debug:

```shell
make qemu-debug-uboot
```

A TCP port will be exposed for GDB debugging. It is typically 25501. The `.gdbinit` file at the root of the repo may be helpful for GDB configuration.
