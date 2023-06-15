// string.c
#include <utilities/string.h>
#include <kmem.h>

void *memset(void *dst, int val, size_t siz)
{
    char *p = (char *)dst;
    while (siz > 0)
        *(p++) = val, siz--;
    return dst;
}

void *memcpy(void *dst, const void *src, size_t siz)
{
    char *dst_ptr = (char *)dst;
    const char *src_ptr = (char *)src;
    while (siz > 0)
        *(dst_ptr++) = *(src_ptr++), siz--;
    return dst;
}

void *memmove(void *dst, const void *src, size_t siz)
{
    if (dst == src)
        return dst;
    char *p1 = (char *)dst;
    const char *p2 = (const char *)src;
    if (dst < src)
        memcpy(dst, src, siz);
    else
    {
        p1 = (char *)dst + siz - 1;
        p2 = (const char *)src + siz - 1;
        while (siz > 0)
            *(p1--) = *(p2--), siz--;
    }
    return dst;
}

int memcmp(const void *lhs, const void *rhs, size_t siz)
{
    unsigned char *lhs_ptr = (unsigned char *)lhs;
    unsigned char *rhs_ptr = (unsigned char *)rhs;
    for (size_t i = 0; i != siz; i++)
        if (*(lhs_ptr + i) < *(rhs_ptr + i))
            return -1;
        else if (*(lhs_ptr + i) > *(rhs_ptr + i))
            return 1;
    return 0;
}

void *memchr(const void *ptr, int ch, size_t siz)
{
    unsigned char c = (unsigned char)ch;
    unsigned char *p = (unsigned char *)ptr;
    for (size_t i = 0; i != siz; i++)
        if (p[i] == c)
            return p + i;
    return NULL;
}

size_t strlen(const char *s)
{
    if (s == NULL)
        return 0;
    size_t ret = 0;
    while (*(s + ret) != '\0')
        ret++;
    return ret;
}

size_t strnlen(const char *str, size_t strsz)
{
    if (str == NULL)
        return 0;
    size_t ret = 0;
    while (ret < strsz && *(str + ret) != '\0')
        ret++;
    return ret;
}

char *strchr(const char *str, int ch)
{
    while (*str != '\0' && *str != (char)ch)
        str++;
    if (*str == '\0')
        return NULL;
    else
        return (char *)str;
}

char *strrchr(const char *str, int ch)
{
    const char *tail = str + strlen(str) - 1;
    while (tail > str && *tail != (char)ch)
        tail--;
    if (*tail != (char)ch)
        return NULL;
    else
        return (char *)tail;
}

char *strcpy(char *dst, const char *src)
{
    char *ret = dst;
    while (*src != '\0')
        *(dst++) = *(src++);
    *dst = '\0';
    return ret;
}

char *strncpy(char *dst, const char *src, size_t siz)
{
    char *ret = dst;
    while (siz-- && *src != '\0')
        *(dst++) = *(src++);
    *dst = '\0';
    return ret;
}

int strcmp(const char *lhs, const char *rhs)
{
    while (*lhs == *rhs && *lhs != '\0')
        lhs++, rhs++;
    return *lhs - *rhs;
}

int strncmp(const char *lhs, const char *rhs, size_t siz)
{
    while (*lhs == *rhs && *lhs != '\0' && siz--)
        lhs++, rhs++;
    return *lhs - *rhs;
}

static inline unsigned char __tolower(unsigned char c)
{
    if ('A' <= c && c <= 'Z')
        c -= 'A' - 'a';
    return c;
}

static const char *__parse_integer_fixup_radix(const char *s, uint *basep)
{
    if (s[0] == '0')
    {
        int ch = __tolower(s[1]);
        if (ch == 'x')
            *basep = 16, s += 2;
        else if (!*basep)
            *basep = 8;
    }
    if (!*basep)
        *basep = 10;
    return s;
}

static uint __decode_digit(int ch)
{
    ch = __tolower(ch);
    return ch <= '9' ? ch - '0' : ch - 'a' + 0xa;
}

ulong strtoul(const char *str, char **endptr, uint base)
{
    ulong result = 0;
    uint value;
    str = __parse_integer_fixup_radix(str, &base);
    while (value = __decode_digit(*str), value < base)
    {
        result = result * base + value;
        str++;
    }
    if (endptr)
        *endptr = (char *)str;
    return result;
}