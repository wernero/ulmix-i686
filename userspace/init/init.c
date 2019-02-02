#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/utsname.h>

#define SHELL "/bin/ush" // ulmix shell

static void start_program(char *filename);

int main(int argc, char *argv[])
{
    if (getpid() != 1)
    {
        printf("init should only be run by the kernel\n");
        return 1;
    }

    int fd = open("/dev/tty1", O_RDWR);
    if (fd < 0)
        exit(-fd);

    printf("init argc=%d\n", argc);

    struct utsname uts;
    uname(&uts);

    printf("%s %s %s    \n"
           "Copyright (C) 2018-2019\n"
           "Written by Alexander Ulmer\n\n",
           uts.sysname, uts.version, uts.machine);
    start_program(SHELL);

    return 0;
}

static void start_program(char *filename)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        // start shell
        if (execve(filename, NULL, NULL) < 0)
        {
            puts("error: execve() returned an error");
            //printf("init: cannot start %s: %s", filename, strerror(errno));
        }
    }
    // printf("initd: forked pid %d\n");
}
