#ifndef UNISTD_H
#define UNISTD_H

#include <sys/types.h>

// syscalls
ssize_t     read    (int fd, char *buf, size_t len);
ssize_t     write   (int fd, char *buf, size_t len);
int         close   (int fd);
int         execve  (const char *filename, char *const argv[], char *const envp[]);

#endif // UNISTD_H
