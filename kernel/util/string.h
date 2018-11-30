#ifndef STRING_H
#define STRING_H

#include "util/types.h"

size_t strlen(const char *str);
void strcpy(char *dest, char *src);
void reverse(char *s);
void itoa(int n, char *buf);
void itoxa(int n, char *buf);
void strcat(char *dest, char *src);
void bsize(uint32_t n, char *buf);

#endif // STRING_H
