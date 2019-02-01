#ifndef KDEBUG_H
#define KDEBUG_H

#include <log.h>

#define KDEBUG_KLOG 0   // print to kernel log
#define KDEBUG_BRK  1   // set breakpoint

void kdebug_init(void);

#endif // KDEBUG_H
