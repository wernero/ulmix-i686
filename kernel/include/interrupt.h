#ifndef IRQ_H
#define IRQ_H

int register_irq(unsigned irq, void (*handler)(void));
int unregister_irq(unsigned irq);

#endif // IRQ_H
