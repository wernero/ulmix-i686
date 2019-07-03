#include <interrupt.h>
#include <types.h>

typedef void (*func_t)();

static func_t irq_handlers[256];

int register_irq(unsigned irq, void (*handler)())
{
   if (irq_handlers[irq] == NULL)
   {
       irq_handlers[irq] = handler;
       return 0;
   }

   return -1;
}

int unregister_irq(unsigned irq)
{
    if (irq_handlers[irq] != NULL)
    {
        irq_handlers[irq] = NULL;
        return 0;
    }

    return -1;
}

void irq_handler(int irq)
{
    if (irq_handlers[irq] != NULL)
        irq_handlers[irq]();
}
