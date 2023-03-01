// parameters.h

#ifndef PARAMETERS

#define PARAMETERS

// Parameters;

#define MAX_CPU 8

// Kernel memory size;
// 64MB by default;
#define KMEM_SIZE 0x4000000
#define KMEM_PAGE_COUNT (KMEM_SIZE >> PAGE_SIZE_OFFSET)

// Virtual memory mode;
// SATP_MODE_NONE 0
// SATP_MODE_SV39 8
// SATP_MODE_SV48 9
// SATP_MODE_SV57 10
// SATP_MODE_SV64 11
#define VMEM_MODE 8L
#define VMEM_LEVEL 2

// Support toggles;

#define SBI_LEGACY_SUPPORT

#endif