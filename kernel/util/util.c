#include "util.h"



void *memset(void *mem, uint8_t value, size_t len)
{
    void *ret = mem;
    while (len--)
    {
        *((uint8_t*)mem) = value;
        mem++;
    }
    return ret;
}

void *memcpy(void *dest, void *src, size_t n)
{
    void *ret = dest;
    while (n--)
    {
        *(uint8_t*)(dest++) = *(uint8_t*)(src++);
    }
    return ret;
}

void *memmove(void *dest, void *src, size_t n)
{
    int i;
    char tmp[n];

    for (i = 0; i < n; i++)
    {
        tmp[i] = ((char*)src)[i];
    }

    for (i = 0; i < n; i++)
    {
        ((char*)dest)[i] = tmp[i];
    }

    return dest;
}

void *bzero(void *mem, size_t len)
{
    return memset(mem, 0, len);
}
