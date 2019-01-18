/* ulibc - Ulmix C Library
 * Copyright (C) 2018-2019 The Ulmix Operating System
 * Written by Alexander Ulmer
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

extern int main(int argc, char *argv[]);

FILE *stdin;
FILE *stdout;
FILE *stderr;

void _start(void)
{
    int fd = open("/dev/tty1", O_RDWR);
    if (fd < 0)
        exit(-fd);

    FILE std;
    std.fileno = fd;
    stdin = stdout = stderr = &std;

    int ret = main(0, NULL);
    // setup standard I/O  -- temporary, the
    //stdout = fopen("/dev/tty", "rw");
    //stdin = stderr = stdout;

    exit(ret);
}
