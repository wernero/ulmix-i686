#include <ulmix.h>
#include <debug.h>
#include <types.h>
#include <string.h>

#define PF_LONG 1
#define PF_PAD  2

extern void debug_putchar(char c);
#define kputchar(c) debug_putchar(c)

static void kputs(char *s)
{
    int len = strlen(s);
    while (len--)
        kputchar(*(s++));
}

static void kprintf(const char *fmt, va_list ap)
{
    int flags = 0;
    char *arg_str;
    char sbuf[64];
    unsigned long arg_int;

    int len = strlen(fmt);
    for (int i = 0; i < len; i++) {
        if (fmt[i] == '%') {

            // check for flags
            switch (fmt[++i]) {
            case 'l':
                flags |= PF_LONG;
                break;
            case '0':
                flags |= PF_PAD;
                break;
            default:
                i--;
            }

            // switch between data type
            switch(fmt[++i]) {
            case 'x':
                if (flags & PF_LONG) {
                    arg_int = va_arg(ap, unsigned long);
                } else {
                    arg_int = (unsigned long)va_arg(ap, unsigned int);
                }
                kputs(xtoa(arg_int, sbuf));
                break;

            case 'S':

            case 'p':
                arg_int = va_arg(ap, unsigned long);
                kputs("0x");
                kputs(xtoa(arg_int, sbuf));
                break;

            case 's':
                arg_str = va_arg(ap, char*);
                if (arg_str)
                    kputs(arg_str);
                break;

            case '%':
                kputchar('%');
                break;
            }

            continue;
        }

        kputchar(fmt[i]);
    }

    va_end(ap);
}

void debug(int level, const char *fmt, ...)
{
    if (LOG_LEVEL > level)
        return;

    va_list args;
    va_start(args, fmt);
    kprintf(fmt, args);
    va_end(args);
}
