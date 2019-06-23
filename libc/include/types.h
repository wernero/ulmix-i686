#ifndef TYPES_H
#define TYPES_H

#include <init.h>

#define NULL 0
#define BIT(x)  (1 << (x))

typedef signed char         int8_t;
typedef signed short int    int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

typedef unsigned long       size_t;
typedef signed long         ssize_t;


typedef __builtin_va_list va_list;
#define va_start(v,l)	__builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)
#if !defined(__STRICT_ANSI__) || __STDC_VERSION__ + 0 >= 199900L \
    || __cplusplus + 0 >= 201103L
#define va_copy(d,s)	__builtin_va_copy(d,s)
#endif
#define __va_copy(d,s) __builtin_va_copy(d,s)

#endif // TYPES_H
