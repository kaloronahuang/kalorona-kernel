// parameters.h

#ifndef PARAMETERS

#define PARAMETERS

// Parameters;

#define MAX_CPU 8

// Stack size per CPU for vkernel;
// 8 megabytes as default;
#ifndef VKERNEL_STACK_SIZE
#define VKERNEL_STACK_SIZE 0x800000
#endif

// Support toggles;

#define SBI_LEGACY_SUPPORT

#endif