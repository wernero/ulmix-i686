#include "log.h"
#include "util/util.h"
#include "util/string.h"
#include "sched/sync.h"


extern int kdebug_enabled;

extern void log_puts(int oflags, char *s);
extern void log_putchar(int oflags, char c);

static void vsprintf(int oflags, const char *format, va_list ap)
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
                log_puts(oflags, strbuf);
                break;
            case 's':
                str = va_arg(ap, char*);
                log_puts(oflags, str);
                break;
            case 'x':
            case 'X':
                n = va_arg(ap, uint32_t);
                itoxa(n, strbuf);
                log_puts(oflags, strbuf);
                break;
            case 'S':
                n = va_arg(ap, uint32_t);
                bsize(n, strbuf);
                log_puts(oflags, strbuf);
                break;
            case '%':
                log_putchar(oflags, '%');
                break;
            default:
                log_putchar(oflags, '?');
            }

            continue;
        }

        log_putchar(oflags, format[i]);
    }
    va_end(ap);
}

void klog(loglevel_t lvl, const char *format, ...)
{
    if (kdebug_enabled)
    {
        int output_flags = OUT_SERIAL;
        if (lvl == KLOG_PANIC ||
                lvl == KLOG_INFO ||
                lvl == KLOG_WARN ||
                lvl == KLOG_FAILURE)
            output_flags |= OUT_TTY;

        if  (lvl == KLOG_PANIC)
            log_puts(output_flags, ">>> PANIC <<<\n");

        va_list args;
        va_start(args, format);
        vsprintf(output_flags, format, args);
        va_end(args);
        log_putchar(output_flags, '\n');
    }

    if (lvl == KLOG_PANIC || lvl == KLOG_FAILURE)
    {
        // kernel panic, stop operating
        cli();
        hlt();
    }
}

