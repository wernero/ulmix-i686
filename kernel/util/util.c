#include "util.h"



void *memset(void *mem, uint8_t value, size_t len)
{
    void *ret = mem;
    while (len--)
    {
        *(uint8_t*)(mem++) = value;
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

void *bzero(void *mem, size_t len)
{
    return memset(mem, 0, len);
}
