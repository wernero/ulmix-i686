#ifndef OPEN_H
#define OPEN_H

#include "vfscore.h"

int open_file(struct direntry_struct *node, int flags);
int open_device(struct direntry_struct *node, int flags);

#endif // OPEN_H
