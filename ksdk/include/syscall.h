// syscall.h

#ifndef SYSCALL_H

#include <kernel/include/types.h>
#include <kernel/include/syscall.h>

#ifndef __ASSEMBLER__

void sys_ecall(
    ulong arg0,
    ulong arg1,
    ulong arg2,
    ulong arg3,
    ulong arg4,
    ulong arg5,
    ulong arg6,
    ulong ecall_type);

#endif

#endif
