#include "string.h"

size_t strlen(const char *str)
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

void strcat(char *dest, char *src)
{
    while (*(dest++) != 0);
    dest--;
    while (*src != 0)
    {
        *(dest++) = *(src++);
    }
    *dest = 0;
}


void bsize(uint32_t n, char *buf)
{
    char *unit;
    uint32_t d;
    if ((d = (n / (1024*1024*1024))) > 0)
        unit = "G";
    else if ((d = (n / (1024*1024))) > 0)
        unit = "M";
    else if ((d = (n / 1024)) > 0)
        unit = "K";
    else
    {
        unit = "B";
        d = n;
    }

    itoa(d, buf);
    strcat(buf, unit);
}

static char get_letter(int n)
{
    if (n < 10)
        return '0' + n;
    return 'a' - 10 + n;
}

static void _itoa(uint32_t n, char *str, uint32_t base)
{
    char *buf = str;
    if (n == 0)
    {
        strcpy(buf, "0");
        return;
    }

    int last_div = 1;
    int div = base;

    while (n / last_div != 0)
    {
        *(buf++) = get_letter((n % div) / last_div);
        last_div = div;
        div *= base;
    }

    *buf = '\0';
    reverse(str);
}

void strcpy(char *dest, char *src)
{
    while (*src)
    {
        *(dest++) = *(src++);
    }
    *dest = 0;
}


void itoa(int n, char *buf)
{
    _itoa(n, buf, 10);
}

void itoxa(uint32_t n, char *str)
{
    char *buf = str;
    for (int i = 0; i < 8; i++)
    {
        *(buf++) = get_letter((n >> i*4) & 0xf);
    }
    *buf = 0;

    reverse(str);
}
