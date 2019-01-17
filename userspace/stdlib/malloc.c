/* ulibc - Ulmix C Library
 * Copyright (C) 2018-2019 The Ulmix Operating System
 * Written by Alexander Ulmer
 */

#include <stdlib.h>

void *malloc(size_t size)
{
    /* not implemented yet */
    return NULL;
}

void free(void *mem)
{
    /* not implemented yet */
}

void *calloc(size_t nmemb, size_t size)
{
    return malloc(nmemb * size);
}

void *realloc(void *ptr, size_t size)
{
    /* not implemented yet */
    return NULL;
}
