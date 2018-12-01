#include "log.h"
#include "util/util.h"
#include "util/string.h"
#include "drivers/serial.h"


static int initialized = 0;

static void vsprintf(uint16_t ttys, const char *format, va_list ap)
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
                serial_write(ttys, strbuf, strlen(strbuf));
                break;
            case 's':
                str = va_arg(ap, char*);
                serial_write(ttys, str, strlen(str));
                break;
            case 'x':
            case 'X':
                n = va_arg(ap, uint32_t);
                itoxa(n, strbuf);
                serial_write(ttys, strbuf, strlen(strbuf));
                break;
            case 'S':
                n = va_arg(ap, uint32_t);
                bsize(n, strbuf);
                serial_write(ttys, strbuf, strlen(strbuf));
                break;
            case '%':
                serial_putchar(ttys, '%');
                break;
            default:
                serial_putchar(ttys, '?');
            }

            continue;
        }

        serial_putchar(ttys, format[i]);
    }
    va_end(ap);
}

void klog(loglevel_t lvl, const char *format, ...)
{
    if (!initialized)
    {
        serial_open(TTYS1, 0);
    }



    va_list args;
    va_start(args, format);
    vsprintf(TTYS1, format, args);
    va_end(args);
    serial_putchar(TTYS1, '\n');

    if (lvl == KLOG_PANIC || lvl == KLOG_FAILURE)
    {
        // kernel panic, stop operating
        cli();
        hlt();
    }
}
