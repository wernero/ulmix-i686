#ifndef FOPS_H
#define FOPS_H

#include <types.h>

struct fops_struct
{
    ssize_t (*read)(char *buffer, size_t count, size_t offset);
    ssize_t (*write)(char *buffer, size_t count, size_t offset);
};

#endif // FOPS_H
