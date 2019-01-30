#ifndef PATH_H
#define PATH_H

#include "vfscore.h"

int namei(char *path, struct direntry_struct **node);
int get_pathname(struct direntry_struct *node, char *buf);

#endif // PATH_H
