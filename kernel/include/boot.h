// boot.h
#include <types.h>
#include <device.h>

#ifndef BOOT

#define BOOT

extern ulong boot_hartid;
extern int booted;

extern void __entry();
void kernel_entrypoint(ulong hartid, struct fdt_header *fdt_addr);
void kernel_main();

#endif