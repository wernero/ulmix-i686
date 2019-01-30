#ifndef PATH_H
#define PATH_H

#include "vfscore.h"

int namei(char *path, struct direntry_struct **node);
int sys_getcwd(char *buf, size_t size);

#endif // PATH_H
