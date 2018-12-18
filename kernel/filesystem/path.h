#ifndef PATH_H
#define PATH_H

#include <filesystem/vfscore.h>

int namei(char *path, struct direntry_struct **node);

#endif // PATH_H
