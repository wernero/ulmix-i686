#ifndef OPEN_H
#define OPEN_H

#include "vfscore.h"

int open_file(struct direntry_struct *node, int flags);
int open_by_major(int major, int minor, int flags, struct direntry_struct *node);
int open_device(struct direntry_struct *node, int flags);

#endif // OPEN_H
