#ifndef EXEC_H
#define EXEC_H

#include <memory/paging.h>


/*
 * The execve() system call replaces the currently executing program image
 * with a new program loaded from the filesystem.
 *
 * This is usually used in combination with fork().
 *
 * Memory layout
 * _________________    0
 * | Kernel static |
 * |---------------|    16M     -|
 * |    .TEXT      |             |
 * |   .RODATA     |             |
 * |---------------|             |
 * |    .DATA      |             |
 * |---------------|             |
 * |     .BSS      |             |
 * |---------------|             |
 * |     Heap      |              > User space
 * |...............|             |
 * |               |             |
 * |               |             |
 * |...............|             |
 * |     Stack     |             |
 * |---------------|             |
 * |  argv - env   |             |
 * |---------------|    3GiB    -|
 * |  Kernel heap  |
 * |_______________|    4GiB
 */
int sys_execve(char *filename, char *argv[], char *envp[]);
int kfexec(char *img_path, char *description);

#endif // EXEC_H
