// printf.c
#include <console.h>
#include <asm/sbi.h>
#include <stdarg.h>
#include <types.h>

static const char *digits = "0123456789abcddef";

void print_char(char c)
{
    // TODO: Waiting for UART support;
    // Using SBI ecall for temporary use;
    sbi_legacy_console_putchar(c);
}

int print_str(char *s)
{
    if (s == NULL)
        return -1;
    while (*(s) != '\0')
        print_char(*(s++));
    return 0;
}

void print_uint16_by_base(uint16 x, uint16 base)
{
    char buf[16];
    buf[0] = '0', buf[1] = '\0';
    size_t cur = 0;
    uint16 tmp = x;
    while (tmp != 0)
    {
        buf[cur++] = digits[tmp % base];
        tmp /= base;
    }
    size_t lptr = 0, rptr = cur - 1;
    while (lptr < rptr)
    {
        char ex_tmp = buf[lptr];
        buf[lptr] = buf[rptr];
        buf[rptr] = ex_tmp;
        lptr++, rptr--;
    }
    buf[cur] = '\0';
    print_str(buf);
}

void print_uint16(uint16 x) { print_uint16_by_base(x, 10); }

void print_uint32_by_base(uint32 x, uint32 base)
{
    char buf[16];
    buf[0] = '0', buf[1] = '\0';
    size_t cur = 0;
    uint32 tmp = x;
    while (tmp != 0)
    {
        buf[cur++] = digits[tmp % base];
        tmp /= base;
    }
    size_t lptr = 0, rptr = cur - 1;
    while (lptr < rptr)
    {
        char ex_tmp = buf[lptr];
        buf[lptr] = buf[rptr];
        buf[rptr] = ex_tmp;
        lptr++, rptr--;
    }
    buf[cur] = '\0';
    print_str(buf);
}

void print_uint32(uint32 x) { print_uint32_by_base(x, 10); }

void print_uint64_by_base(uint64 x, uint64 base)
{
    char buf[32];
    buf[0] = '0', buf[1] = '\0';
    size_t cur = 0;
    uint64 tmp = x;
    while (tmp != 0)
    {
        buf[cur++] = digits[tmp % base];
        tmp /= base;
    }
    size_t lptr = 0, rptr = cur - 1;
    while (lptr < rptr)
    {
        char ex_tmp = buf[lptr];
        buf[lptr] = buf[rptr];
        buf[rptr] = ex_tmp;
        lptr++, rptr--;
    }
    buf[cur] = '\0';
    print_str(buf);
}

void print_uint64(uint64 x) { print_uint64_by_base(x, 10); }

void print_int16_by_base(int16 x, int16 base)
{
    char buf[16];
    buf[0] = '0', buf[1] = '\0';
    size_t cur = 0;
    int16 tmp = x;
    while (tmp != 0)
    {
        buf[cur++] = digits[tmp % base];
        tmp /= base;
    }
    size_t lptr = 0, rptr = cur - 1;
    while (lptr < rptr)
    {
        char ex_tmp = buf[lptr];
        buf[lptr] = buf[rptr];
        buf[rptr] = ex_tmp;
        lptr++, rptr--;
    }
    buf[cur] = '\0';
    if (x < 0)
        print_char('-');
    print_str(buf);
}

void print_int16(int16 x) { print_int16_by_base(x, 10); }

void print_int32_by_base(int32 x, int32 base)
{
    char buf[16];
    buf[0] = '0', buf[1] = '\0';
    size_t cur = 0;
    int32 tmp = x;
    while (tmp != 0)
    {
        buf[cur++] = digits[tmp % base];
        tmp /= base;
    }
    size_t lptr = 0, rptr = cur - 1;
    while (lptr < rptr)
    {
        char ex_tmp = buf[lptr];
        buf[lptr] = buf[rptr];
        buf[rptr] = ex_tmp;
        lptr++, rptr--;
    }
    buf[cur] = '\0';
    if (x < 0)
        print_char('-');
    print_str(buf);
}

void print_int32(int32 x) { print_int32_by_base(x, 10); }

void print_int64_by_base(int64 x, int64 base)
{
    char buf[32];
    buf[0] = '0', buf[1] = '\0';
    size_t cur = 0;
    int64 tmp = x;
    while (tmp != 0)
    {
        buf[cur++] = digits[tmp % base];
        tmp /= base;
    }
    size_t lptr = 0, rptr = cur - 1;
    while (lptr < rptr)
    {
        char ex_tmp = buf[lptr];
        buf[lptr] = buf[rptr];
        buf[rptr] = ex_tmp;
        lptr++, rptr--;
    }
    buf[cur] = '\0';
    if (x < 0)
        print_char('-');
    print_str(buf);
}

void print_int64(int64 x) { print_int64_by_base(x, 10); }