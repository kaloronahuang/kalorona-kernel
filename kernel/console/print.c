// printf.c
#include <console.h>
#include <asm/sbi.h>
#include <stdarg.h>
#include <types.h>

static const char *digits = "0123456789abcdef";

struct console_lock_struct console_lock;

void console_init()
{
    spinlock_init(&(console_lock.lock), "console_lock");
    console_lock.locking_enabled = 1;
    printf("[console]console initialized.\n");
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
        print_char('-'), x = -x;
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
    va_list ap;
    va_start(ap, fmt);
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
            print_int(va_arg(ap, int32), 10);
            break;
        case 'x':
            print_int(va_arg(ap, int32), 16);
            break;
        case 'p':
            print_char('0'), print_char('x');
            print_uint(va_arg(ap, uint64), 16);
            break;
        case 'u':
            print_uint(va_arg(ap, uint32), 10);
            break;
        case 's':
            char *s = va_arg(ap, char *);
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

void print_buffer(void *buf, size_t size)
{
    if (console_lock.locking_enabled)
        spinlock_acquire(&(console_lock.lock));
    for (size_t i = 0; i != size; i++)
    {
        if (*((uint8 *)(buf + i)) < 16)
            print_char('0');
        print_uint(*((uint8 *)(buf + i)), 16);
        print_char(' ');
        if ((i + 1) % 16 == 0 && i != size - 1)
            print_char('\n');
    }
    print_char('\n');
    if (console_lock.locking_enabled)
        spinlock_release(&(console_lock.lock));
}
