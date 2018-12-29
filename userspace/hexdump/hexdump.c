#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


void print_ascii(unsigned char *buf, int len)
{
    putchar(' ');
    putchar('|');

    for (int i = 0; i < len; i++)
    {
        if (buf[i] >= 32 && buf[i] < 127)
            putchar(buf[i]);
        else
            putchar('.');
    }

    putchar('|');
    putchar('\n');
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("hexdump: error: too few arguments\n");
        return 1;
    }

    char *filename = argv[1];

    int fd;
    if ((fd = open(filename, O_RDONLY)) < 0)
    {
        printf("hexdump: errno %d: cannot open file\n", errno);
        return 1;
    }

    int len;
    unsigned long offset = 0;
    unsigned char buf[16];
    while ((len = read(fd, buf, 16)) > 0)
    {
        // print address
        printf("0x%08lx:  ", offset);

        // print data
        for (int i = 0; i < len; i++)
        {
            printf("%02x ", buf[i]);
            if (i == 7)
                putchar(' ');
        }


        int rem = 16 - len;
        while (rem--)
            printf("   ");

        if (len < 8)
            putchar(' ');

        // print ascii:
        print_ascii(buf, len);

        offset += 16;
        if (len < 16)
            break;
    }

    close(fd);
    return 0;
}
