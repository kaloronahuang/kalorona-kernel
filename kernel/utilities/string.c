// string.c
#include <utilities/string.h>

size_t strlen(const char *s)
{
    size_t ret = 0;
    while (*(s + ret) != '\0')
        ++ret;
    return ret;
}

void memset(void *dst, char val, size_t siz)
{
    char *pos = dst;
    for (size_t i = 0; i < siz; i++)
        pos[i] = val;
}