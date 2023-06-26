# syscall.S

#include <syscall.h>

.global sys_ecall
sys_ecall:
    ecall
    ret

.global exit
exit:
    li a7, SYSCALL_CODE_EXIT
    ecall
    ret

.global fork
fork:
    li a7, SYSCALL_CODE_FORK
    ecall
    ret

.global kill
kill:
    li a7, SYSCALL_CODE_KILL
    ecall
    ret

.global sleep
sleep:
    li a7, SYSCALL_CODE_SLEEP
    ecall
    ret

.global uptime
uptime:
    li a7, SYSCALL_CODE_UPTIME
    ecall
    ret

.global wait
wait:
    li a7, SYSCALL_CODE_WAIT
    ecall
    ret

.global sbrk
sbrk:
    li a7, SYSCALL_CODE_SBRK
    ecall
    ret

.global getpid
getpid:
    li a7, SYSCALL_CODE_GETPID
    ecall
    ret
