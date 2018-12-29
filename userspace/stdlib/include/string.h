#ifndef STRING_H
#define STRING_H

#include <sys/types.h>

size_t strlen(const char *str);

char *strerror(int errnum);

int strcmp(char *s1, char *s2);
int strncmp(char *s1, char *s2, size_t n);

#endif // STRING_H
