// kcrt.c
#include <stdlib.h>

void _start()
{
    extern int main();
    exit(main());
}
