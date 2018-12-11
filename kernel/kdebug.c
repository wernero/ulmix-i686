#include "kdebug.h"
#include "log.h"
#include "util/util.h"
#include "drivers/serial.h"
#include "sched/sync.h"
#include "video/video.h"

int kdebug_enabled = 0;
struct tty_struct *debug_tty = NULL;


void kdebug_init()
{
    debug_tty = tty_open();
    if (debug_tty != NULL)
        tty_focus(debug_tty);

    serial_open(TTYS1, 0);

    kdebug_enabled = 1;
}

void log_puts(int oflags, char *s)
{
    int l = strlen(s);
    if (oflags & OUT_TTY)       tty_write(debug_tty, s, l);
    if (oflags & OUT_SERIAL)    serial_write(TTYS1, s, l);
}

void log_putchar(int oflags, char c)
{
    if (oflags & OUT_TTY)       tty_putchar(debug_tty, c);
    if (oflags & OUT_SERIAL)    serial_putchar(TTYS1, c);
}
