#include <debug.h>
#include <types.h>
#include <exception.h>

void page_fault(void *fault_addr, unsigned long err_code)
{
    kprintf(" *** PAGE FAULT INFO *** \n"
            "   addr      = %p\n"
            "   cause     = %s\n"
            "   operation = %s\n"
            "   runlevel  = %s\n"
            "   op fetch  = %s\n\n",
            fault_addr,
            (err_code & BIT(0)) ? "protection" : "non-present page",
            (err_code & BIT(1)) ? "write" : "read",
            (err_code & BIT(2)) ? "user" : "kernel",
            (err_code & BIT(4)) ? "yes" : "no"
    );
}

extern void *fault_addr;
extern unsigned long fault_code;

int generic_exception(unsigned id)
{
    if (id == EXC_PAGEFAULT)
    {
        page_fault(fault_addr, fault_code);
    }

    return EXC_STATUS_FATAL;
}
