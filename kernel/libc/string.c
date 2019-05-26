#include <string.h>

void *memset(void *mem, unsigned char value, size_t n)
{
    unsigned char *mptr = mem;

    while (n--)
        *(mptr++) = value;

    return mem;
}

void *memcpy(void *dest, void *src, size_t n)
{
    unsigned char *dstp = dest;
    unsigned char *srcp = src;

    while (n--)
        *dstp++ = *srcp++;

    return dest;
}

void *memmove(void *dest, void *src, size_t n);

void *bzero(void *mem, size_t n)
{
    unsigned char *mptr = mem;

    while (n--)
        *(mptr++) = 0;

    return mem;
}

char *stpcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
char *strchr(const char *s, int c);

int strcmp(const char *s1, const char *s2);

char *strcpy(char *dest, const char *src)
{
    char *ret = dest;

    while (*src)
        *dest++ = *src++;

    return ret;
}

size_t strlen(const char *s)
{
    size_t len = 0;

    while (*(s++))
        ++len;

    return len;
}

char *strncat(char *dest, const char *src, size_t n);
char *strncmp(const char *s1, const char *s2, size_t n);
char *strncpy(char *dest, const char *src, size_t n);
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

char *xtoa(unsigned long x, char *buf)
{
    char *str = buf;
    int c, begin = 1;
    for (int i = (sizeof(unsigned long) * 2) -1; i >= 0; i--) {
        c = nchar((x >> i*4) & 0xf);
        if (c == '0' && begin) {
            continue;
        } else {
            begin = 0;
        }
        *(buf++) = c;
    }
    *buf = 0;
    return str;
}

