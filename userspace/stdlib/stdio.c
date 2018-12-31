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
    vprintf(stdout, format, args);
    va_end(args);
}

void vprintf(FILE *stream, const char *format, va_list ap)
{
    for (int i = 0; format[i] != '\0'; i++)
    {
        if (format[i] == '%')
        {
            if (format[++i] == '%')
                putchar('%');


            continue;
        }

        putchar(format[i]);
    }
    va_end(ap);
}
