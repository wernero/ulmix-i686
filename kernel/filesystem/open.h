#ifndef OPEN_H
#define OPEN_H

#include "vfscore.h"

int open_file(struct direntry_struct *node, int flags);
struct file_struct *kopen_device(ftype_t type, int major, int minor, int flags);
int open_device(struct direntry_struct *node, int flags);
void kclose(struct file_struct *fd);

#endif // OPEN_H
