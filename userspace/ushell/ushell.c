#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/keyboard.h>
#include <sys/ioctl.h>

int main(void)
{
    int fd = open("/dev/kbd", O_RDONLY);
    if (fd < 0)
    {
        puts("could not open keyboard device\n");
        return 1;
    }

    // put keyboard driver into ASCII mode
    if (ioctl(fd, KBD_SELECT_MODE, KBD_MODE_ASCII)< 0)
    {
        puts("ioctl failed\n");
        return 2;
    }

    char c;
    while (1)
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

