#include <asm.h>
#include <debug.h>
#include <types.h>
#include <exception.h>

#include "idt.h"

struct exc_context
{
    unsigned long id;

    unsigned long edi;
    unsigned long esi;
    unsigned long ebp;
    unsigned long esp;
    unsigned long ebx;
    unsigned long edx;
    unsigned long ecx;
    unsigned long eax;

    unsigned long gs;
    unsigned long fs;
    unsigned long es;
    unsigned long ds;

    unsigned long error_code;
    unsigned long eip;
} __attribute__((packed));


void *fault_addr;
unsigned long fault_code;

extern int generic_exception(unsigned id);

void exc_handler(struct exc_context *context)
{
    fault_code = context->error_code;
    fault_addr = (void*)context->eip;

    if (context->id == EXC_PAGEFAULT)
    {
        __asm__ volatile ("mov %%cr2, %0" : "=r"(fault_addr));
    }

    if (generic_exception(context->id) == EXC_STATUS_OK)
        return;

    kprintf("\n === KERNEL PANIC === \n"
            "   unhandled %s exception (#%d)\n"
            "   eax=%p, ebx=%p, ecx=%p, edx=%p\n"
            "   esi=%p, edi=%p, ebp=%p, esp=%p\n"
            "   eip=%p\n\n"
            "execution halted.",
            exceptions[context->id], context->id,
            context->eax, context->ebx, context->ecx, context->edx,
            context->esi, context->edi, context->ebp, context->esp,
            context->eip);

    cli();
    hlt();
}
