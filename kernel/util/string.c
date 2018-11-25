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

void itoa(int n, char *buf)
{
    int last_mult = 1;
    for (int mult = 10; mult < 1000000000; mult *= 10)
    {
        if (n / mult == 0)
        {
            break;
        }

        int rst = n % mult;
        rst /= last_mult;
        *(buf++) = '0' + rst;

        last_mult = mult;
    }

    *(buf++) = 0;
}

void itoxa(int n, char *buf)
{
    *(buf++) = '0';
    *(buf++) = 'x';

    int last_mult = 1;
    for (int mult = 10; mult < 1000000000; mult *= 10)
    {
        if (n / mult == 0)
        {
            break;
        }

        int rst = n % mult;
        rst /= last_mult;
        *(buf++) = '0' + rst;

        last_mult = mult;
    }

    *(buf++) = 0;
}
