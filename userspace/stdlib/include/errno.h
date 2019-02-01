/* ulibc - Ulmix C Library
 * Copyright (C) 2018-2019 The Ulmix Operating System
 * Written by Alexander Ulmer
 */

#ifndef ERRNO_H
#define ERRNO_H

#define SUCCESS 0

#define EPERM   1
#define ENOENT  2
#define ESRCH   3
#define EINTR   4
#define EIO     5
#define ENXIO   6
#define E2BIG   7
#define ENOEXEC 8

#define EBADF   9
#define EAGAIN  11
#define ENOMEM  12

#define ENOTDIR 20
#define EISDIR  21
#define EINVAL  22
#define ENFILE  23
#define EMFILE  24

#define ENOSYS  38
#define ENOTSUP 95

#define ENOBUFS 105

extern int errno;

#endif // ERRNO_H
