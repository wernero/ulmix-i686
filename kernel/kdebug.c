#include "kdebug.h"
#include "log.h"
#include "util/util.h"
#include "drivers/serial.h"
#include "sched/sync.h"

int kdebug_enabled = 0;
mutex_t *log_mutex;

#define DEBUG_SERIAL 1

void kdebug_init()
{
    serial_open(TTYS1, 0);
    kdebug_enabled = 1;
    log_mutex = mutex();
}

void log_puts(char *s)
{
    serial_write(TTYS1, s, strlen(s));
}

void log_putchar(char c)
{
    serial_putchar(TTYS1, c);
}
