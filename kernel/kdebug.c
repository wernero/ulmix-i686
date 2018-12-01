#include "kdebug.h"
#include "log.h"
#include "util/util.h"
#include "drivers/serial.h"

int kdebug_enabled = 0;

void kdebug_init()
{
    serial_open(TTYS1, 0);
    kdebug_enabled = 1;
}

void kdebug_break(void)
{

}
