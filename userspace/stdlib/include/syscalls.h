#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <sys/types.h>

void exit(void);
int open(char *path, int flags);
int close(int fd);
ssize_t read(int fd, char *buf, size_t len);
ssize_t write(int fd, char *buf, size_t len);

unsigned long sysc(
        unsigned int    id,
        unsigned int    arg1,
        unsigned int    arg2,
        unsigned int    arg3,
        unsigned int    arg4);

#endif // SYSCALLS_H
