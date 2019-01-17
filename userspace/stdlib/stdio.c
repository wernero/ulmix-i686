/* ulibc - Ulmix C Library
 * Copyright (C) 2018-2019 The Ulmix Operating System
 * Written by Alexander Ulmer
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

void puts(char *s)
{
    write(stdout->fileno, s, strlen(s));
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


int printf(const char *format, ...)
{
    char buf[256];
    size_t len;

    va_list args;
    va_start(args, format);
    len = sprintf(buf, format, args);
    va_end(args);

    return write(stdout->fileno, buf, len);
}

