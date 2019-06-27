#include <string.h>

void *memset(void *mem, unsigned char value, unsigned long n)
{
    unsigned char *mptr = mem;

    while (n--)
        *(mptr++) = value;

    return mem;
}

void *memcpy(void *dest, void *src, unsigned long n)
{
    unsigned char *dstp = dest;
    unsigned char *srcp = src;

    while (n--)
        *dstp++ = *srcp++;

    return dest;
}

void *memmove(void *dest, void *src, unsigned long n);

void bzero(void *s, unsigned long n)
{
    unsigned char *ptr = s;
    while (n--)
        *ptr++ = 0;
}

char *stpcpy(char *dest, const char *src);

char *strcat(char *dest, const char *src)
{
    char *dest_ptr = dest;
    while (*dest++);
    dest--;
    while (*src)
        *dest++ = *src++;
    *dest = 0;
    return dest_ptr;
}

char *strchr(const char *s, int c);

int strcmp(const char *s1, const char *s2);

char *strcpy(char *dest, const char *src)
{
    char *ret = dest;

    while (*src)
        *dest++ = *src++;
    *dest = 0;

    return ret;
}

unsigned long strlen(const char *s)
{
    unsigned long len = 0;

    while (*(s++))
        ++len;

    return len;
}

char *strncat(char *dest, const char *src, unsigned long n);
char *strncmp(const char *s1, const char *s2, unsigned long n);
char *strncpy(char *dest, const char *src, unsigned long n);
char *strstr(const char *haystack, const char *needle);

char *reverse(char *s)
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }

    return s;
}

static char nchar(int x)
{
    if (x < 0)
        x = -x;

    if (x < 10)
        return '0' + x;
    return 'a' + x - 10;
}

int atoi(const char *nptr)
{
    int x = 0;
    unsigned len = strlen(nptr);
    int base = 1;
    for (int i = len - 1; i >= 0; i--)
    {
        x += ((nptr[i]) - '0') * base;
        base *= 10;
    }
    return x;
}

char *itoa(long x, char *buf, int pad)
{
    if (x < 0)
    {
        x = -x;
    }

    utoa((unsigned)x, buf, pad);
    return buf;
}

char *utoa(unsigned long x, char *buf, int pad)
{
    if (pad < 1)
        pad = 1;

    long i, rem;
    for (i = 0; x > 0; i++)
    {
        rem = x % 10;
        buf[i] = nchar(rem);

        x /= 10;
    }

    pad -= i;
    while (pad-- > 0)
        buf[i++] = '0';
    buf[i] = 0;

    reverse(buf);
    return buf;
}

char *xtoa(unsigned long x, char *buf, int pad)
{
    if (pad < 1)
        pad = 1;

    char *str = buf;
    int c, begin = 1;
    for (int i = (sizeof(unsigned long) * 2) -1; i >= 0; i--) {
        c = nchar((x >> i*4) & 0xf);
        if (c == '0' && begin && i >= pad) {
            continue;
        } else {
            begin = 0;
        }
        *(buf++) = c;
    }
    *buf = 0;
    return str;
}
