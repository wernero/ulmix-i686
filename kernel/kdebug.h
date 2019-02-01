#ifndef KDEBUG_H
#define KDEBUG_H

#include <log.h>

#define KDEBUG_KLOG 0   // print to kernel log
#define KDEBUG_BRK  1   // set breakpoint

int sys_kdebug(unsigned long request, unsigned long arg1, unsigned long arg2);
void kdebug_init(void);

#endif // KDEBUG_H
