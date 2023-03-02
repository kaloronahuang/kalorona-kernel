// printf.c
#include <console.h>
#include <asm/sbi.h>
#include <stdarg.h>
#include <types.h>

static const char *digits = "0123456789abcddef";

struct console_lock_struct console_lock;

void console_init()
{
    spinlock_init(&(console_lock.lock), "console_lock");
    console_lock.locking_enabled = 1;
}

static void print_char(char c)
{
    // TODO: Waiting for UART support;
    // Using SBI ecall for temporary use;
    sbi_legacy_console_putchar(c);
}

static void print_uint(uint64 x, uint64 base)
{
    if (x == 0)
        print_char('0');
    else
    {
        char buf[64];
        int ptr = 0;
        while (x)
            buf[ptr++] = digits[x % base], x /= base;
        while (ptr--)
            print_char(buf[ptr]);
    }
}

static void print_int(int64 x, uint64 base)
{
    if (x < 0)
        putchar('-'), x = -x;
    print_uint(x, base);
}

void putchar(const char c)
{
    if (console_lock.locking_enabled)
        spinlock_acquire(&(console_lock.lock));
    print_char(c);
    if (console_lock.locking_enabled)
        spinlock_release(&(console_lock.lock));
}

int printf(const char *fmt, ...)
{
    if (fmt == NULL)
        return -1;
    if (console_lock.locking_enabled)
        spinlock_acquire(&(console_lock.lock));

    char *param_ptr = (&fmt) + sizeof(fmt);
    for (int i = 0; fmt[i]; i++)
    {
        if (fmt[i] != '%')
        {
            print_char(fmt[i]);
            continue;
        }
        char c = fmt[++i];
        if (c == '\0')
        {
            print_char('%');
            break;
        }
        switch (c)
        {
        case 'd':
            print_int(*((int32 *)param_ptr), 10);
            param_ptr += sizeof(int32);
            break;
        case 'x':
            print_int(*((int32 *)param_ptr), 16);
            param_ptr += sizeof(int32);
            break;
        case 'p':
            print_char('0'), print_char('x');
            print_uint(*((uint64 *)param_ptr), 16);
            param_ptr += sizeof(uint64);
            break;
        case 'u':
            print_uint(*((uint32 *)param_ptr), 10);
            param_ptr += sizeof(uint32);
            break;
        case 's':
            char *s = *((char **)param_ptr);
            if (s == NULL)
                s = "(null)";
            for (; *s; s++)
                print_char(*s);
            break;
        case '%':
            print_char('%');
            break;
        default:
            print_char('%'), print_char(c);
            break;
        }
    }

    if (console_lock.locking_enabled)
        spinlock_release(&(console_lock.lock));
    return 0;
}