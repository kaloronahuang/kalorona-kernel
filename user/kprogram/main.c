// main.c
#include <stdlib.h>
#include <syscall.h>

int main(int argc, char *argv[])
{
    while (1)
        sys_ecall(0, 0, 0, 0, 0, 0, 0, 0);
    return 0;
}
