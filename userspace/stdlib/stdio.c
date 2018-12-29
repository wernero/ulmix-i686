#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define FD_KLOG   912
#define FD_STDIN  0
#define FD_STDOUT 0
#define FD_STDERR 1



void puts(char *s)
{
    write(stdout->fileno, s, strlen(s) + 1);
}

void putchar(char c)
{
    write(stdout->fileno, &c, 1);
}

char getchar(void)
{
    char c;
    if (read(stdin->fileno, &c, 1) < 1)
        return EOF;
    return c;
}


void printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(stdout, format, args);
    va_end(args);
}

#define VPRINTF_BUF 128
void vsprintf(FILE *stream, const char *format, va_list ap)
{
    char buf[VPRINTF_BUF];
    int buf_index = 0;

    for (int i = 0; format[i] != '\0'; i++)
    {
        if (buf_index == VPRINTF_BUF)
        {
            // flush the buffer
            write(stream->fileno, buf, buf_index);
            buf_index = 0;
        }

        if (format[i] == '%')
        {
            if (format[++i] == '%')
                buf[buf_index++] = '%';


            continue;
        }

        buf[buf_index++] = format[i];
    }

    write(stream->fileno, buf, buf_index);
    va_end(ap);
}
