/* ulibc - Ulmix C Library
 * Copyright (C) 2018-2019 The Ulmix Operating System
 * Written by Alexander Ulmer
 */

#ifndef STRING_H
#define STRING_H

#include <sys/types.h>


char *strerror(int errnum);

size_t strlen(const char *str);
int strcmp(char *s1, char *s2);
int strncmp(char *s1, char *s2, size_t n);
char *strsize(unsigned long n, char *buf);
size_t strcat(char *dest, char *src);
void reverse(char *s);
char *itoa(int n, char *str, int base);
void strcpy(char *dest, char *src);

#endif // STRING_H
