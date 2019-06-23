#ifndef IRQ_H
#define IRQ_H

void register_irq(unsigned irq, void (*handler)(void));
void unregister_irq(unsigned irq);

#endif // IRQ_H
