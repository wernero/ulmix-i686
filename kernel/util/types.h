#ifndef ULMIX_TYPES_H
#define ULMIX_TYPES_H

typedef signed char             int8_t;
typedef signed short int        int16_t;
typedef signed int              int32_t;
typedef signed long long int    int64_t;

typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;
typedef unsigned long long int  uint64_t;

typedef unsigned long int       uid_t;
typedef unsigned long int       gid_t;


typedef uint32_t size_t;
typedef int32_t ssize_t;

typedef struct
{
    unsigned char addr[6];
} __attribute__((packed)) mac_t;

typedef uint32_t ipaddr_t;


typedef __builtin_va_list va_list;
#define va_start(v,l)	__builtin_va_start(v,l)
#define va_end(v)	__builtin_va_end(v)
#define va_arg(v,l)	__builtin_va_arg(v,l)
#if !defined(__STRICT_ANSI__) || __STDC_VERSION__ + 0 >= 199900L \
    || __cplusplus + 0 >= 201103L
#define va_copy(d,s)	__builtin_va_copy(d,s)
#endif
#define __va_copy(d,s)	__builtin_va_copy(d,s)

#endif
