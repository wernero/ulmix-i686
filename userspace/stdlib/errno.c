#include <errno.h>

static int last_errno;

int _errno(void)
{
    return last_errno;
}

void _set_errno(int err)
{
    last_errno = err;
}
