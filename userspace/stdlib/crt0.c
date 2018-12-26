#include <stdio.h>
#include <syscalls.h>
#include <stdlib.h>

FILE *stdin;
FILE *stdout;
FILE *stderr;

int main(int argc, char *argv[]);

void _start(void)
{
    // setup standard I/O  -- temporary, the
    stdout = fopen("/dev/tty", "rw");
    stdin = stderr = stdout;

    // call main function
    int return_code;
    return_code = main(0, NULL);

    // don't just return
    exit(return_code);
}
