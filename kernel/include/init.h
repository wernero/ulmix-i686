#ifndef INIT_H
#define INIT_H

#define __cold      __attribute__((cold))

#define __init      __attribute__((section(".init.text"))) __cold
#define __initdata  __attribute__((section(".init.data")))
#define __initconst __attribute__((section(".init.rodata")))

#endif // INIT_H
