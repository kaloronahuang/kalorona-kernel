// main.c
#include <stdlib.h>
#include <syscall.h>

int main(int argc, char *argv[])
{
    int pid = getpid();
    int x = uptime();
    sys_ecall(pid, x, 1, 0, 0, 0, 0, 0);
    sleep(10);
    x = uptime();
    sys_ecall(pid, x, 1, 0, 0, 0, 0, 0);

    int child_pid = fork();
    if (child_pid == 0)
    {
        // child;
        x = uptime();
        sys_ecall(child_pid, x, 2, 0, 0, 0, 0, 0);
        sleep(20);
        x = uptime();
        sys_ecall(child_pid, x, 2, 0, 0, 0, 0, 0);
        return 4;
    }
    else
    {
        int child_code = 0;
        x = uptime();
        sys_ecall(pid, x, 3, 0, 0, 0, 0, 0);
        wait(&child_code);
        x = uptime();
        sys_ecall(pid, x, 3, child_code, 0, 0, 0, 0);
    }
    return 0;
}
