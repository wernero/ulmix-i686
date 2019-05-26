#include "util.h"



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
