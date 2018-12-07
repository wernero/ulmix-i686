#ifndef EXEC_H
#define EXEC_H

/*
 * The execve() system call replaces the currently executing program image
 * with a new program loaded from the filesystem.
 *
 * This is usually used in combination with fork().
 *
 * Memory layout
 * _________________    0
 * | Kernel static |
 * |---------------|    16M
 * |    .TEXT      |
 * |   .RODATA     |
 * |---------------|
 * |    .DATA      |
 * |---------------|
 * |     .BSS      |
 * |---------------|
 * |    Stack      |
 * |...............|
 * |               |
 * |               |
 * |...............|
 * |     Heap      |
 * |---------------|
 * |  argv - env   |
 * |---------------|    3GiB
 * |  Kernel heap  |
 * |_______________|    4GiB
 */
int sc_execve(const char *filename, char *argv[], char *envp[]);

#endif // EXEC_H
