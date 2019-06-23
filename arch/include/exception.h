#ifndef EXCEPTION_H
#define EXCEPTION_H

#define EXC_STATUS_FATAL   1
#define EXC_STATUS_OK      0

#define EXC_DIVZERO     0
#define EXC_DEBUG       1
#define EXC_NMI         2
#define EXC_BRKPOINT    3
#define EXC_OVERFLOW    4
#define EXC_BOUNDS      5
#define EXC_INV_OPCODE  6
#define EXC_NO_COPROC   7
#define EXC_DOUBLE_FLT  8
#define EXC_SEG_OVRRUN  9
#define EXC_INV_TSS     10
#define EXC_SEG_N_PRES  11
#define EXC_STACK_FLT   12
#define EXC_PROTECTION  13
#define EXC_PAGEFAULT   14
#define EXC_INTERRUPT   15
#define EXC_COPROC      16
#define EXC_ALIGNMENT   17
#define EXC_MACHINE_CHK 18
#define EXC_SIMD        19

static const char* const exceptions[] =
{
    "divide by zero",
    "debug",
    "non-maskable interrupt",
    "breakpoint",
    "into detected overflow",
    "out of bounds",
    "invalid opcode"
    "no coprocessor",
    "double fault",
    "coprocessor segment overrun",
    "broken task state segment",
    "segment not present",
    "stack fault",
    "general protection fault",
    "page fault",
    "unknown interrupt",
    "coprocessor fault",
    "alignment check",
    "machine check",
    "SIMD",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved"
};

#endif // EXCEPTION_H
