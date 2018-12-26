#ifndef SYSCALLS_H
#define SYSCALLS_H

void exit(void);
int open(char *path, int flags);
int close(int fd);
ssize_t read(int fd, char *buf, size_t len);
ssize_t write(int fd, char *buf, size_t len);

#endif // SYSCALLS_H
