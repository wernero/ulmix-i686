#include <debug.h>
#include <types.h>
#include <string.h>

#define FMT_LONG BIT(0)
#define PTR_PADDING 8

/*static char dmesg_buffer[8192];
static int dmesg_index = 0;

static void kputc(char c)
{
    dmesg_buffer[dmesg_index++] = c;
    if (dmesg_index >= 8192)
        dmesg_index = 0;
}*/

extern void debug_putchar(char c);
#define kputc(c) debug_putchar(c)

static void kputs(char *s)
{
    while (*s)
        kputc(*s++);
}

static void get_flags(int *iptr, int *fmt_flags, int *padding, const char *fmt_str)
{
    int i = *iptr;
    for (;;)
    {
        i++;

        if (fmt_str[i] == '0')
        {
            char buf[8];
            char *buf_ptr = buf;

            while (fmt_str[i] >= '0' && fmt_str[i] <= '9')
                *buf_ptr++ = fmt_str[i++];
            *buf_ptr = 0;

            *padding = atoi(buf);
        }
        else if (fmt_str[i] == 'l')
        {
            *fmt_flags |= FMT_LONG;
        }
        else
        {
            break;
        }
    }
    *iptr = i;
}

static char scale(unsigned long *sz)
{
    if (*sz < (1024 * 10))
        return 'B';

    if (*sz < (1024 * 1024 * 10))
    {
        *sz /= 1024;
        return 'K';
    }

    if (*sz < (unsigned long)-1)
    {
        *sz /= (1024 * 1024);
        return 'M';
    }

    *sz /= (1024 * 1024 * 1024);
    return 'G';
}

static void do_kprintf(const char *fmt, va_list ap)
{
    char fmt_char;
    int fmt_flags;
    int padding;

    void *ptr;
    long fmt_long;
    char fmt_buf[128];

    unsigned long fmt_len = strlen(fmt);
    for (int i = 0; i < fmt_len; i++)
    {
        fmt_flags = 0;
        padding = 1;

        if (fmt[i] == '%')
        {
            get_flags(&i, &fmt_flags, &padding, fmt);
            fmt_char = fmt[i];

            switch (fmt_char)
            {
            case '%':
                kputc('%');
                break;

            case 'd':
                if ((fmt_flags & FMT_LONG))
                    fmt_long = va_arg(ap, long);
                else
                    fmt_long = (long)va_arg(ap, int);
                kputs(itoa(fmt_long, fmt_buf, padding));
                break;

            case 'u':
                if ((fmt_flags & FMT_LONG))
                    fmt_long = va_arg(ap, long);
                else
                    fmt_long = (long)va_arg(ap, int);
                kputs(utoa((unsigned)fmt_long, fmt_buf, padding));
                break;

            case 's':
                ptr = va_arg(ap, char *);
                if (ptr)
                    kputs(ptr);
                break;

            case 'c':
                kputc(va_arg(ap, int));
                break;

            case 'p':
                kputs("0x");
                fmt_flags |= FMT_LONG;
                padding = PTR_PADDING;
                // break; // fallthrough

            case 'x':
                if ((fmt_flags & FMT_LONG))
                    fmt_long = va_arg(ap, long);
                else
                    fmt_long = (long)va_arg(ap, int);
                kputs(xtoa((unsigned)fmt_long, fmt_buf, padding));
                break;

            case 'S':
                if ((fmt_flags & FMT_LONG))
                    fmt_long = va_arg(ap, long);
                else
                    fmt_long = (long)va_arg(ap, int);

                char scale_c = scale((unsigned long*)&fmt_long);
                kputs(utoa((unsigned)fmt_long, fmt_buf, padding));
                kputc(scale_c);
                break;

            default:
                break;
            }
        }
        else
        {
            kputc(fmt[i]);
        }
    }
}

void kprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    do_kprintf(fmt, args);
    va_end(args);
}
