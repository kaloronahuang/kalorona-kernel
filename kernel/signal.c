// signal.c
#include <signal.h>
#include <console.h>

void panic(const char *s)
{
    printf("[panic]%s\n", s);
    for (;;)
    {
    }
}