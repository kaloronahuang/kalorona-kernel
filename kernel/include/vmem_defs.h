// vmem_defs.h
#include <parameters.h>

#ifndef VMEM_LAYOUT

#define VMEM_LAYOUT

// RISC-V Virtual memory mode;
#define VMEM_MODE_NONE 0ULL

#define VMEM_MODE_SV39 8ULL
#define VMEM_SV39_LEN 39ULL
#define VMEM_SV39_N_VPN 3

#define VMEM_MODE_SV48 9ULL
#define VMEM_SV48_LEN 48ULL
#define VMEM_SV48_N_VPN 4

#define VMEM_MODE_SV57 10ULL
#define VMEM_SV57_LEN 57ULL
#define VMEM_SV57_N_VPN 5

// By default, the mode is SV39;
// It can be altered by the macro option by the compiler;
#ifndef VMEM_MODE
#define VMEM_MODE VMEM_MODE_SV39
#endif

#if VMEM_MODE == VMEM_MODE_SV39

#define VA_LEN VMEM_SV39_LEN
#define VA_N_VPN VMEM_SV39_N_VPN
#define VMEM_KERNEL_SPACE_VA_BEGIN 0xFFFFFFC000000000

#elif VMEM_MODE == VMEM_MODE_SV48

#define VA_LEN VMEM_SV48_LEN
#define VA_N_VPN VMEM_SV48_N_VPN
#define VMEM_KERNEL_SPACE_VA_BEGIN 0xFFFF800000000000

#elif VMEM_MODE == VMEM_MODE_SV57

#define VA_LEN VMEM_SV57_LEN
#define VA_N_VPN VMEM_SV57_N_VPN
#define VMEM_KERNEL_SPACE_VA_BEGIN 0xFF00000000000000

#endif

#endif