#include "log.h"
#include <util/util.h>
#include <sched/sync.h>


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



void hexdump(loglevel_t lvl, const void* data, size_t size) {

    int output_flags = OUT_SERIAL;
    if (lvl == KLOG_PANIC ||
    lvl == KLOG_INFO ||
    lvl == KLOG_WARN ||
    lvl == KLOG_FAILURE)
      output_flags |= OUT_TTY;


    klog(KLOG_INFO, "***hexdump*** address=%x, length=%x 0123456789ABCDEF",
    data,
    size
    );

    char ascii[17];
    char strbuf[64];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i) {

    if(!(i % 0x10)) {
      itoxa(i, strbuf);
      log_puts(output_flags, strbuf);
      log_puts(output_flags, " | ");
    }

    itoxa8(((unsigned char*)data)[i], strbuf);
    log_puts(output_flags, strbuf);

    if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
        ascii[i % 16] = ((unsigned char*)data)[i];
    } else {
        ascii[i % 16] = '.';
    }
    if ((i+1) % 8 == 0 || i+1 == size) {
        log_puts(output_flags, " ");
        if ((i+1) % 16 == 0) {
        log_puts(output_flags,"|  ");
        log_puts(output_flags, ascii);
        log_puts(output_flags, " \n");
        } else if (i+1 == size) {
        ascii[(i+1) % 16] = '\0';
        if ((i+1) % 16 <= 8) {
            log_puts(output_flags, " ");
        }
        for (j = (i+1) % 16; j < 16; ++j) {
            log_puts(output_flags, "   ");
        }
        log_puts(output_flags,"|  ");
        log_puts(output_flags, ascii);
        log_puts(output_flags, " \n"); 	    }
    }
    }
}
