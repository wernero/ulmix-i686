/* ulibc - Ulmix C Library
 * Copyright (C) 2018-2019 The Ulmix Operating System
 * Written by Alexander Ulmer
 */

#ifndef FCNTL_H
#define FCNTL_H

typedef enum
{
    O_RDONLY    =0x00,
    O_APPEND    =0x01,
    O_CREAT     =0x02,
    O_RDWR      =0x04,
    O_WRONLY    =0x08
} openflags_t;

int open(char *path, int flags);

#endif // FCNTL_H
