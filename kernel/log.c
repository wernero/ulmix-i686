#include "log.h"
#include "util/util.h"
#include "util/string.h"
#include "sched/sync.h"

extern int kdebug_enabled;

extern void log_puts(char *s);
extern void log_putchar(char c);

extern mutex_t *log_mutex;

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
    if (kdebug_enabled)
    {
        mutex_lock(log_mutex);

        va_list args;
        va_start(args, format);
        vsprintf(format, args);
        va_end(args);
        log_putchar('\n');

        mutex_unlock(log_mutex);
    }

    if (lvl == KLOG_PANIC || lvl == KLOG_FAILURE)
    {
        // kernel panic, stop operating
        cli();
        hlt();
    }
}
