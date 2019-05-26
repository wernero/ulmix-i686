#ifndef STRING_H
#define STRING_H

void *memset(void *ptr, uint8_t value, size_t n);
void *memcpy(void *dest, void *src, size_t n);
void *memmove(void *dest, void *src, size_t n);
void *bzero(void *ptr, size_t n);

#endif // STRING_H
