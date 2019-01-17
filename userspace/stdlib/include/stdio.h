/* ulibc - Ulmix C Library
 * Copyright (C) 2018-2019 The Ulmix Operating System
 * Written by Alexander Ulmer
 */

#ifndef STDIO_H
#define STDIO_H


#define EOF -1

typedef struct file_struct
{
    int fileno;
} FILE;

extern FILE *stdout;
extern FILE *stdin;
extern FILE *stderr;


void puts(char *s);
void putchar(char c);
char getchar(void);

typedef __builtin_va_list va_list;
#define va_start(v,l)	__builtin_va_start(v,l)
#define va_end(v)	__builtin_va_end(v)
#define va_arg(v,l)	__builtin_va_arg(v,l)
#if !defined(__STRICT_ANSI__) || __STDC_VERSION__ + 0 >= 199900L \
    || __cplusplus + 0 >= 201103L
#define va_copy(d,s)	__builtin_va_copy(d,s)
#endif
#define __va_copy(d,s)	__builtin_va_copy(d,s)

int printf(const char *format, ...);
int sprintf(char *buf, const char *format, va_list ap);

#endif // STDIO_H
