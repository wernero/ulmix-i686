/* ulibc - Ulmix C Library
 * Copyright (C) 2018-2019 The Ulmix Operating System
 * Written by Alexander Ulmer
 */

#include <errno.h>

int actual_errno;

int __errno(void)
{
    return actual_errno;
}
