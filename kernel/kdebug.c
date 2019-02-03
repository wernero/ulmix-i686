#include "kdebug.h"
#include "log.h"
#include "util/util.h"
#include "drivers/serial.h"
#include "sched/sync.h"

#include <video/tty.h>

int kdebug_enabled = 0;
mutex_t *log_mutex;


void kdebug_init()
{
    serial_open(TTYS1, 0);
    log_mutex = mutex();
    kdebug_enabled = 1;
}

void log_puts(char *s)
{
    int l = strlen(s);
    //if (oflags & OUT_TTY)       tty_kernel_write(s, l);
    serial_write(TTYS1, s, l);
}

void log_putchar(char c)
{
    //if (oflags & OUT_TTY)       tty_kernel_putchar(c);
    serial_putchar(TTYS1, c);
}

int sys_kdebug(unsigned long request, unsigned long arg1, unsigned long arg2)
{
    if (request == KDEBUG_KLOG)
    {
        klog(KLOG_DEBUG, (char*)arg1);
        return SUCCESS;
    }

    if (request == KDEBUG_HEXDUMP)
    {
        hexdump(KLOG_DEBUG, (void*)arg1, arg2);
        return SUCCESS;
    }

    if (request == KDEBUG_HEAPDUMP)
    {
        heap_dump();
        return SUCCESS;
    }

    return -ENOSYS;
}
