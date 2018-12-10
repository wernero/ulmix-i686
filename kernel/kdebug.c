#include "kdebug.h"
#include "log.h"
#include "util/util.h"
#include "drivers/serial.h"
#include "sched/sync.h"
#include "video/video.h"

int kdebug_enabled = 0;
mutex_t *log_mutex;
struct tty_struct *debug_tty = NULL;

#define DEBUG_SERIAL 1

void kdebug_init()
{
    debug_tty = tty_open();
    if (debug_tty != NULL)
        tty_focus(debug_tty);

    serial_open(TTYS1, 0);
    log_mutex = mutex();

    kdebug_enabled = 1;
}

void log_puts(char *s)
{
    int l = strlen(s);
    tty_write(debug_tty, s, l);
    serial_write(TTYS1, s, l);
}

void log_putchar(char c)
{
    tty_putchar(debug_tty, c);
    serial_putchar(TTYS1, c);
}
