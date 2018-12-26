#include <stdio.h>
#include <string.h>
#include <syscalls.h>

#define FD_STDIN  0
#define FD_STDOUT 0
#define FD_STDERR 1

void puts(char *s)
{
    write(FD_STDOUT, s, strlen(s) + 1);
}

void putchar(char c)
{
    write(FD_STDOUT, &c, 1);
}

char getchar(void)
{
    char c;
    if (read(FD_STDIN, &c, 1) < 1)
        return EOF;
    return c;
}
