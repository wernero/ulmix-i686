#include "exceptions.h"
#include "log.h"
#include "util/util.h"
#include "sched/task.h"
#include <sched/process.h>

extern thread_t *current_thread;

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

extern void page_fault_handler(uint32_t error, uint32_t fault_addr);


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

void exc_handler(struct exc_context_struct *context)
{
    uint32_t error = context->error_code;
    uint32_t exc = context->exc;

    klog(KLOG_EXCEPTION, "EXCEPTION: #%d -> %s, error=0x%x, eip=0x%x", exc, exceptions[exc], error, context->eip);
    klog(KLOG_DEBUG, "eax=0x%x, ebx=0x%x, ecx=0x%x, edx=0x%x",
         context->eax, context->ebx, context->ecx, context->edx);
    klog(KLOG_DEBUG, "esi=0x%x, edi=0x%x, esp=0x%x, ebp=0x%x",
         context->esi, context->edi, context->esp, context->ebp);

    if (exc == 13)
    {
        if (current_thread != NULL)
        {
            klog(KLOG_INFO, "kill [%d]: protection violation", current_thread->process->pid);
            kill_process(current_thread->process);
            return;
        }
    }

    // Page Fault
    if (exc == 14)
    {
        uint32_t cr2;
        __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
        klog(KLOG_DEBUG, "faulting address: 0x%x", cr2);

        page_fault_handler(error, cr2);
        return;
    }


    // Kernel panic
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
    irq_install_raw_handler(14, exc14, INT_GATE | INT_SUPV); // critical, should not be interrupted
    irq_install_raw_handler(16, exc16, INT_TRAP | INT_SUPV);
    irq_install_raw_handler(17, exc17, INT_TRAP | INT_SUPV);
    irq_install_raw_handler(18, exc18, INT_TRAP | INT_SUPV);
    irq_install_raw_handler(19, exc19, INT_TRAP | INT_SUPV);
    irq_install_raw_handler(20, exc20, INT_TRAP | INT_SUPV);
    irq_install_raw_handler(30, exc30, INT_TRAP | INT_SUPV);
}
