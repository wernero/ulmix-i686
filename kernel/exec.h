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
 * |    Stack      |              > User space
 * |...............|             |
 * |               |             |
 * |               |             |
 * |...............|             |
 * |     Heap      |             |
 * |---------------|             |
 * |  argv - env   |             |
 * |---------------|    3GiB    -|
 * |  Kernel heap  |
 * |_______________|    4GiB
 */
int sc_execve(char *filename, char *argv[], char *envp[]);
int exec_load_img(pagedir_t *pd, char *img_path, void **entry);

#endif // EXEC_H
