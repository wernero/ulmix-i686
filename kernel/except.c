#include <debug.h>

#define EXC_FATAL   1
#define EXC_OK      0

int exc_undefined(int no)
{
    return EXC_FATAL;
}

int exc_protection(void)
{
    return EXC_FATAL;
}

int exc_pagefault(unsigned long error, unsigned long address)
{
    debug(L_DEBUG, "CPU: page fault at 0x%x\n", address);
    return EXC_FATAL;
}
