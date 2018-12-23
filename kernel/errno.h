#ifndef ERRNO_H
#define ERRNO_H

#define EPERM   1
#define ENOENT  2
#define ESRCH   3
#define EINTR   4
#define EIO     5			// I/O error
#define ENXIO   6
#define E2BIG   7

#define EBADF   9

#define ENOTDIR 20
#define EISDIR  21

#define ENFILE  23			// Too many files open in system
#define EMFILE  24			// Too many open files

#define ENOSYS  38			// Invalid system call number
#define ENOTSUP 95

#define ENOBUFS 105

int _errno(void);
#define errno _errno();

#endif // ERRNO_H
