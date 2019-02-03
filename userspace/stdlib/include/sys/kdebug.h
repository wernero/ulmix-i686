#ifndef KDEBUG_H
#define KDEBUG_H

#include <sys/types.h>

#define KDEBUG_KLOG     0   // print to kernel log
#define KDEBUG_CONT     1   // set breakpoint and report context at that point
#define KDEBUG_HEXDUMP  2   // hexdump
#define KDEBUG_HEAPDUMP 3   // heap dump

int kdebug(unsigned long request, unsigned long arg1, unsigned long arg2);

struct kcontext
{
    unsigned long eax;
    unsigned long ebx;
    unsigned long ecx;
    unsigned long edx;
    unsigned long esi;
    unsigned long edi;

    unsigned long ebp;
    unsigned long esp;

    unsigned long eip;
};

static inline int kbreak(void *addr, struct kcontext *context) // set breakpoint in the kernel
{
    return kdebug(KDEBUG_CONT, (unsigned long)addr, (unsigned long)context);
}

static inline int khexdump(void *addr, size_t size)
{
    return kdebug(KDEBUG_HEXDUMP, (unsigned long)addr, size);
}

static inline int kheapdump(void)
{
    return kdebug(KDEBUG_HEAPDUMP, 0, 0);
}

#endif // KDEBUG_H
