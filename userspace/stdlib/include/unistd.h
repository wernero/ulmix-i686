/* ulibc - Ulmix C Library
 * Copyright (C) 2018-2019 The Ulmix Operating System
 * Written by Alexander Ulmer
 */

#ifndef UNISTD_H
#define UNISTD_H

#include <sys/types.h>

// syscalls
ssize_t     read    (int fd, void *buf, size_t len);
ssize_t     write   (int fd, void *buf, size_t len);
int         close   (int fd);

pid_t       fork    (void);
int         execve  (const char *filename, char *const argv[], char *const envp[]);


#endif // UNISTD_H
