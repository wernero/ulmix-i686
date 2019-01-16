#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
int main(void)
{
    puts("ULMIX shell\n\n $ ");

    int fd = open("/dev/kbd", O_RDONLY);
    if (fd < 0)
    {
        puts("fd returned < 0");
        return 1;
    }

    char c;
    int f = 4;
    while (f--)
    {
        if (read(fd, &c, 1) < 1)
        {
            puts("read returned < 1, break\n");
            break;
        }
        putchar(c);
        if (c == '\n')
            puts("$ ");
    }

    return 0;
}

