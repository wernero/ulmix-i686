#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "interrupts.h"

struct exc_context_struct
{
    uint32_t exc;

    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    uint32_t error_code;
    uint32_t eip;
} __attribute__((packed));

void setup_exception_handlers(void);

#endif // EXCEPTIONS_H
