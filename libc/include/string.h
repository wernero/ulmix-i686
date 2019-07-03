#ifndef STRING_H
#define STRING_H

void *memset(void *ptr, unsigned char value, unsigned long n);
void *memcpy(void *dest, void *src, unsigned long n);
void *memmove(void *dest, void *src, unsigned long n);
void bzero(void *s, unsigned long n);

char *stpcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
char *strchr(const char *s, int c);

int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
unsigned long strlen(const char *s);
char *strncat(char *dest, const char *src, unsigned long n);
char *strncmp(const char *s1, const char *s2, unsigned long n);
char *strncpy(char *dest, const char *src, unsigned long n);
char *strstr(const char *haystack, const char *needle);

char *reverse(char *s);

int atoi(const char *nptr);
long atol(const char *nptr);

char *utoa(unsigned long x, char *buf, int pad);
char *itoa(long x, char *buf, int pad);
char *xtoa(unsigned long x, char *buf, int pad);

const char *strerror(unsigned err);

#endif // STRING_H
