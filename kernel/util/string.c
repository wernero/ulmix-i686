#include "string.h"

size_t strlen(char *str)
{
    size_t len = 0;
    while (*str++)
    {
        len++;
    }
    return len;
}

void reverse(char *s)
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--)
    {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

static void _itoa(int n, char *str, int base)
{
    int i = 0, is_negative = 0;
    if (n == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (n < 0 && base == 10)
    {
        is_negative = 1;
        n = -n;
    }

    while (n != 0)
    {
        int rem = n % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        n = n/base;
    }

    if (is_negative)
    {
        str[i++] = '-';
    }

    str[i] = '\0';
    reverse(str);
}


void itoa(int n, char *buf)
{
    _itoa(n, buf, 10);
}

void itoxa(int n, char *buf)
{
    _itoa(n, buf, 16);
}
