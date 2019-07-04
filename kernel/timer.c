#include <time.h>
#include <errno.h>

void irq_timer(void)
{

}

int sys_gettimeofday(struct timeval *tv, struct timezone *tz)
{
    return -ENOSYS;
}
