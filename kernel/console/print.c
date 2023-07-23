// printf.c
#include <console.h>
#include <asm/sbi.h>
#include <stdarg.h>
#include <types.h>
#include <hal/uart_io.h>

static const char *digits = "0123456789abcdef";

struct console_struct console;

void console_init()
{
    spinlock_init(&(console.lock), "console_lock");
    console.locking_enabled = 1;
    printf("[console]console initialized.\n");
}

static void print_char(char c)
{
    if (console.kernel_stdout_dev == NULL)
        sbi_legacy_console_putchar(c);
    else
        hal_uart_write(console.kernel_stdout_dev, c);
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
    if (console.locking_enabled)
        spinlock_acquire(&(console.lock));
    print_char(c);
    if (console.locking_enabled)
        spinlock_release(&(console.lock));
}

char getchar(void)
{
    if (console.kernel_stdout_dev == NULL)
        return -1;
    else
        return hal_uart_read(console.kernel_stdout_dev);
}

int printf(const char *fmt, ...)
{
    if (fmt == NULL)
        return -1;
    if (console.locking_enabled)
        spinlock_acquire(&(console.lock));
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
        case 'c':
            print_char(va_arg(ap, int));
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

    if (console.locking_enabled)
        spinlock_release(&(console.lock));
    return 0;
}

void print_buffer(void *buf, size_t size)
{
    if (console.locking_enabled)
        spinlock_acquire(&(console.lock));
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
    if (console.locking_enabled)
        spinlock_release(&(console.lock));
}
