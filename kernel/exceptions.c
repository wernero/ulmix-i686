#include "exceptions.h"
#include "log.h"
#include "util/util.h"

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


static const char* const exceptions[] =
{
    "divide by zero",          "debug",                         "non-maskable interrupt",    "breakpoint",
    "into detected overflow",  "out of Bounds",                 "invalid opcode",            "no coprocessor",
    "double fault",            "coprocessor Segment Overrun",   "broken task state segment", "segment not present",
    "stack fault",             "general Protection Fault",      "page fault",                "unknown interrupt",
    "coprocessor fault",       "alignment Check",               "machine check",             "SIMD exception",
    "reserved",                "reserved",                      "reserved",                  "reserved",
    "reserved",                "reserved",                      "reserved",                  "reserved",
    "reserved",                "reserved",                      "reserved",                  "reserved"
};

void exc_handler(uint32_t exc)
{
    klog(KLOG_EXCEPTION, "exception: %s", exceptions[exc]);
    if (exc == 14)
    {
        uint32_t cr2;
        __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
        klog(KLOG_DEBUG, "page fault: faulting address: 0x%x\n", cr2);
    }

    cli();
    hlt();
}

void setup_exception_handlers(void)
{
    irq_install_raw_handler( 0,  exc0, INT_TRAP | INT_SUPV);
    irq_install_raw_handler( 1,  exc1, INT_TRAP | INT_SUPV);
    irq_install_raw_handler( 2,  exc2, INT_TRAP | INT_SUPV);
    irq_install_raw_handler( 3,  exc3, INT_TRAP | INT_SUPV);
    irq_install_raw_handler( 4,  exc4, INT_TRAP | INT_SUPV);
    irq_install_raw_handler( 5,  exc5, INT_TRAP | INT_SUPV);
    irq_install_raw_handler( 6,  exc6, INT_TRAP | INT_SUPV);
    irq_install_raw_handler( 7,  exc7, INT_TRAP | INT_SUPV);
    irq_install_raw_handler( 8,  exc8, INT_TRAP | INT_SUPV);
    irq_install_raw_handler(10, exc10, INT_TRAP | INT_SUPV);
    irq_install_raw_handler(11, exc11, INT_TRAP | INT_SUPV);
    irq_install_raw_handler(12, exc12, INT_TRAP | INT_SUPV);
    irq_install_raw_handler(13, exc13, INT_TRAP | INT_SUPV);
    irq_install_raw_handler(14, exc14, INT_TRAP | INT_SUPV);
    irq_install_raw_handler(16, exc16, INT_TRAP | INT_SUPV);
    irq_install_raw_handler(17, exc17, INT_TRAP | INT_SUPV);
    irq_install_raw_handler(18, exc18, INT_TRAP | INT_SUPV);
    irq_install_raw_handler(19, exc19, INT_TRAP | INT_SUPV);
    irq_install_raw_handler(20, exc20, INT_TRAP | INT_SUPV);
    irq_install_raw_handler(30, exc30, INT_TRAP | INT_SUPV);
}
