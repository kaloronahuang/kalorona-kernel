// parameters.h

#ifndef PARAMETERS

#define PARAMETERS

// Parameters;

#define MAX_CPU 8

// Kernel memory size;
// 64MB by default;
#define KMEM_SIZE 0x4000000
#define KMEM_PAGE_COUNT (KMEM_SIZE >> PAGE_SIZE_OFFSET)

// Support toggles;

#define SBI_LEGACY_SUPPORT

#endif