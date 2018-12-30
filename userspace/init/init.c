#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define SHELL   "/bin/ush" // ulmix shell

static void start_program(char *filename);

int main(void)
{
    printf("ULMIX initd\n");

    start_program(SHELL);

    return 0;
}

static void start_program(char *filename)
{
    pid_t pid = 0;
    if (pid)
    {
        // start shell
        if (execve(filename, NULL, NULL) < 0)
        {
            printf("init: cannot start %s: %s", filename, strerror(errno));
        }
    }
}
