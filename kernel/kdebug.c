#include "kdebug.h"
#include "log.h"
#include "util/util.h"
#include "drivers/serial.h"
#include "sched/sync.h"

#include <video/tty.h>

int kdebug_enabled = 0;


void kdebug_init()
{
    serial_open(TTYS1, 0);

    kdebug_enabled = 1;
}

void log_puts(int oflags, char *s)
{
    int l = strlen(s);
    if (oflags & OUT_TTY)       tty_kernel_write(s, l);
    if (oflags & OUT_SERIAL)    serial_write(TTYS1, s, l);
}

void log_putchar(int oflags, char c)
{
    if (oflags & OUT_TTY)       tty_kernel_putchar(c);
    if (oflags & OUT_SERIAL)    serial_putchar(TTYS1, c);
}
