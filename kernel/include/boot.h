// boot.h
#include <types.h>

#ifndef BOOT

#define BOOT

extern ulong boot_hartid;
extern int booted;

extern void __entry();
void kernel_entrypoint(ulong hartid, ptr_t fdt_addr);

#endif