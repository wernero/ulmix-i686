#include "log.h"
#include "util/util.h"
#include "util/string.h"
#include "drivers/serial.h"

static int initialized = 0;

static void log_puts(char *s)
{
    serial_write(TTYS1, s, strlen(s));
}

static void log_putchar(char c)
{
    serial_putchar(TTYS1, c);
}

static void vsprintf(const char *format, va_list ap)
{
    char strbuf[64];
    uint32_t n;
    char *str;
    for (int i = 0; i < strlen(format); i++)
    {
        if (format[i] == '%')
        {
            switch (format[++i])
            {
            case 'd':
                n = va_arg(ap, uint32_t);
                itoa(n, strbuf);
                log_puts(strbuf);
                break;
            case 's':
                str = va_arg(ap, char*);
                log_puts(str);
                break;
            case 'x':
            case 'X':
                n = va_arg(ap, uint32_t);
                itoxa(n, strbuf);
                log_puts(strbuf);
                break;
            case 'S':
                n = va_arg(ap, uint32_t);
                bsize(n, strbuf);
                log_puts(strbuf);
                break;
            case '%':
                log_putchar('%');
                break;
            default:
                log_putchar('?');
            }

            continue;
        }

        log_putchar(format[i]);
    }
    va_end(ap);
}

void klog(loglevel_t lvl, const char *format, ...)
{
    if (!initialized)
    {
        serial_open(TTYS1, 0);
        initialized = 1;
    }



    va_list args;
    va_start(args, format);
    vsprintf(format, args);
    va_end(args);
    serial_putchar(TTYS1, '\n');

    if (lvl == KLOG_PANIC || lvl == KLOG_FAILURE)
    {
        // kernel panic, stop operating
        cli();
        hlt();
    }
}
