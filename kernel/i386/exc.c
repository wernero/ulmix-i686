#include <asm.h>
#include <types.h>
#include <debug.h>

#include "idt.h"
#include "exc.h"

static const char* const exceptions[] =
{
    "divide by zero",          "debug",                         "non-maskable interrupt",    "breakpoint",
    "into detected overflow",  "out of Bounds",                 "invalid opcode",            "no coprocessor",
    "double fault",            "coprocessor Segment Overrun",   "broken task state segment", "segment not present",
    "stack fault",             "general protection Fault",      "page fault",                "unknown interrupt",
    "coprocessor fault",       "alignment Check",               "machine check",             "SIMD exception",
    "reserved",                "reserved",                      "reserved",                  "reserved",
    "reserved",                "reserved",                      "reserved",                  "reserved",
    "reserved",                "reserved",                      "reserved",                  "reserved"
};

extern int exc_undefined(int no);
extern int exc_protection(void);
extern int exc_pagefault(unsigned long error, unsigned long address);

void exc_handler(struct exc_context_struct *context)
{
    uint32_t error = context->error_code;
    uint32_t exc = context->exc;

    int panic = 0;
    if (exc == 13) {
        panic = exc_protection();
    } else if (exc == 14) {
        uint32_t cr2;
        __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
        panic = exc_pagefault(error, cr2);
    } else {
        debug(L_WARN, "CPU: exception: %s\n", exceptions[exc]);
        panic = exc_undefined(exc);
    }

    if (panic) {
        debug(L_FATAL, "*** PANIC: exception could not be handled\n"
              "exc=%d (%s), error=0x%08x, eip=0x%08x\n"
              "eax=0x%08x, eax=0x%08x, eax=0x%08x, eax=0x%08x,\n"
              "eax=0x%08x, eax=0x%08x, eax=0x%08x, eax=0x%08x\n\n",
              exc, exceptions[exc], error, context->eip,
              context->eax, context->ebx, context->ecx, context->edx,
              context->esi, context->edi, context->esp, context->ebp);

        cli();
        hlt();
    }
}

extern void exc0(void);
extern void exc1(void);
extern void exc2(void);
extern void exc3(void);
extern void exc4(void);
extern void exc5(void);
extern void exc6(void);
extern void exc7(void);
extern void exc8(void);
extern void exc10(void);
extern void exc11(void);
extern void exc12(void);
extern void exc13(void);
extern void exc14(void);
extern void exc16(void);
extern void exc17(void);
extern void exc18(void);
extern void exc19(void);
extern void exc20(void);
extern void exc30(void);

void setup_exc(void)
{
    set_idt_entry(0, exc0, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(1, exc1, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(2, exc2, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(3, exc3, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(4, exc4, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(5, exc5, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(6, exc6, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(7, exc7, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(8, exc8, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(10, exc10, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(11, exc11, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(12, exc12, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(13, exc13, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(14, exc14, INT_PRESENT | INT_GATE | INT_SUPV); // Page fault is a Gate
    set_idt_entry(16, exc16, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(17, exc17, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(18, exc18, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(19, exc19, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(20, exc20, INT_PRESENT | INT_TRAP | INT_SUPV);
    set_idt_entry(30, exc30, INT_PRESENT | INT_TRAP | INT_SUPV);
}
