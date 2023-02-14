// printf.c
#include <console.h>
#include <riscv/sbi.h>
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

int print_uint16_by_base(uint16 x, uint16 base)
{
    if (base < 2)
        return -1;
    while (x != 0)
    {
        uint16 digit = x % base;
        print_char(digits[digit]);
        x /= base;
    }
    return 0;
}

int print_uint16(uint16 x) { return print_uint16_by_base(x, 10); }

int print_uint32_by_base(uint32 x, uint32 base)
{
    if (base < 2)
        return -1;
    while (x != 0)
    {
        uint32 digit = x % base;
        print_char(digits[digit]);
        x /= base;
    }
    return 0;
}

int print_uint32(uint32 x) { return print_uint32_by_base(x, 10); }

int print_uint64_by_base(uint64 x, uint64 base)
{
    if (base < 2)
        return -1;
    while (x != 0)
    {
        uint64 digit = x % base;
        print_char(digits[digit]);
        x /= base;
    }
    return 0;
}

int print_uint64(uint64 x) { return print_uint64_by_base(x, 10); }

int print_int16_by_base(int16 x, int16 base)
{
    if (base < 2)
        return -1;
    while (x != 0)
    {
        int16 digit = x % base;
        print_char(digits[digit]);
        x /= base;
    }
    return 0;
}

int print_int16(int16 x) { return print_int16_by_base(x, 10); }

int print_int32_by_base(int32 x, int32 base)
{
    if (base < 2)
        return -1;
    while (x != 0)
    {
        int32 digit = x % base;
        print_char(digits[digit]);
        x /= base;
    }
    return 0;
}

int print_int32(int32 x) { return print_int32_by_base(x, 10); }

int print_int64_by_base(int64 x, int64 base)
{
    if (base < 2)
        return -1;
    while (x != 0)
    {
        int64 digit = x % base;
        print_char(digits[digit]);
        x /= base;
    }
    return 0;
}

int print_int64(int64 x) { return print_int64_by_base(x, 10); }