#include "uname.h"
#include <errno.h>

int sys_uname(struct utsname *buf)
{
    return -ENOSYS;
}
