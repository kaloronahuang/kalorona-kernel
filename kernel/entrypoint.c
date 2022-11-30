// kalorona-posix
// entrypoint.c
#include "kernel/parameters.h"

__attribute__((aligned(16))) char init_stack[4096 * MAX_CPU];

void kalorona_entrypoint()
{
}
