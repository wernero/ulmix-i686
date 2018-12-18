#include "errno.h"

static int last_errno = 0;

int _errno(void)
{
    return last_errno;
}
