/* ulibc - Ulmix C Library
 * Copyright (C) 2018-2019 The Ulmix Operating System
 * Written by Alexander Ulmer
 */

#include <stdio.h>
#include <string.h>

int sprintf(char *buf, const char *format, va_list ap)
{
    buf[0] = 0;
    size_t length = 0;

    char strbuf[64];
    long n;
    char *str;
    size_t len = 0;

    for (int i = 0; format[i] != '\0'; i++)
    {
        if (format[i] == '%')
        {
            *buf = 0;
            switch (format[++i])
            {
            case 'd':
                n = va_arg(ap, int);
                len = strcat(buf, itoa(n, strbuf, 10));
                break;

            case 's':
                str = va_arg(ap, char*);
                len = strcat(buf, str);
                break;

            case 'x':
            case 'X':
                n = va_arg(ap, unsigned int);
                len = strcat(buf, itoa(n, strbuf, 16));
                break;

            case 'S':
                n = (long)(va_arg(ap, unsigned long));
                len = strcat(buf, strsize((unsigned long)n, strbuf));
                break;

            case '%':
                *buf = '%';
                len = 1;
                break;
            default:
                break;
            }

            length += len;
            buf += len;

            continue;
        }

        *buf++ = format[i];
        length++;
    }
    va_end(ap);

    return length;
}
