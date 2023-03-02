// signal.c
#include <signal.h>
#include <console.h>

void panic(const char *s)
{
    printf(s);
    for (;;)
    {
    }
}