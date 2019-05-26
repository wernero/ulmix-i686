#ifndef STRING_H
#define STRING_H

#include <types.h>

void *memset(void *ptr, uint8_t value, size_t n);
void *memcpy(void *dest, void *src, size_t n);
void *memmove(void *dest, void *src, size_t n);
void *bzero(void *ptr, size_t n);

char *stpcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
char *strchr(const char *s, int c);

int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
size_t strlen(const char *s);
char *strncat(char *dest, const char *src, size_t n);
char *strncmp(const char *s1, const char *s2, size_t n);
char *strncpy(char *dest, const char *src, size_t n);
char *strstr(const char *haystack, const char *needle);

char *reverse(char *s);
char *xtoa(unsigned long x, char *buf);

#endif // STRING_H
