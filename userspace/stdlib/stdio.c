#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define FD_KLOG   912
#define FD_STDIN  0
#define FD_STDOUT 0
#define FD_STDERR 1



void puts(char *s)
{
    write(FD_KLOG, s, strlen(s) + 1);
}

void putchar(char c)
{
    write(FD_KLOG, &c, 1);
}

char getchar(void)
{
    char c;
    if (read(FD_STDIN, &c, 1) < 1)
        return EOF;
    return c;
}
