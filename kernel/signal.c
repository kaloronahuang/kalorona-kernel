// signal.c
#include <signal.h>
#include <console.h>

void panic(const char *s)
{
    print_str(s);
    for (;;)
    {
    }
}