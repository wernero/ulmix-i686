/* ulibc - Ulmix C Library
 * Copyright (C) 2018-2019 The Ulmix Operating System
 * Written by Alexander Ulmer
 */

#ifndef UNISTD_H
#define UNISTD_H

#include <sys/types.h>
#define PATH_MAX  1024

// syscalls
ssize_t     read    (int fd, void *buf, size_t len);
ssize_t     write   (int fd, void *buf, size_t len);
int         close   (int fd);

pid_t       fork    (void);
pid_t       getpid  (void);
int         execve  (const char *filename, char *const argv[], char *const envp[]);

char *      getcwd  (char *buf, size_t size);
int         chdir   (const char *path);

// library functions
char *      getwd   (char *buf);
char *      get_current_dir_name(void);



#endif // UNISTD_H
