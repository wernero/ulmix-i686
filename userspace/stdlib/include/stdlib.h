#ifndef STDLIB_H
#define STDLIB_H

typedef signed long int pid_t;

pid_t fork(void);
void _exit(int err);

#define exit _exit

#endif // STDLIB_H
