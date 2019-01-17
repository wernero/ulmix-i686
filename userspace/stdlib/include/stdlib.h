/* ulibc - Ulmix C Library
 * Copyright (C) 2018-2019 The Ulmix Operating System
 * Written by Alexander Ulmer
 */

#ifndef STDLIB_H
#define STDLIB_H

typedef unsigned long size_t;

#define EXIT_FAILURE    1
#define EXIT_SUCCESS    0
#define NULL            ((void*)0)

/* memory management */
void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *mem);

/* string formatting functions */
double atof(const char *nptr);
int atoi(const char *nptr);
long atol(const char *nptr);
long long atoll(const char *nptr);
double strtod(const char *nptr, char **endptr);
float strtof(const char *nptr, char **endptr);
long double strtold(const char *nptr, char **endptr);
long int strtol(const char *nptr, char **endptr, int base);
long long int strtoll(const char *nptr, char **endptr, int base);
unsigned long int strtoul(const char *nptr, char **endptr, int base);
unsigned long long int strtoull(const char *nptr, char **endptr, int base);

/* random number generation */
#define RAND_MAX -1 // not yet defined
int rand(void);
void srand(unsigned int seed);

/* process control */
void abort(void);
void atexit(void);
void _exit(int err);
#define exit _exit
void at_quick_exit(void);
void quick_exit(void);

/* environment */
char *getenv(const char *name);
int system(const char *command);

/* sorting */
void bsearch(void);
void qsort(void);

/* basic math */
void abs(void);
void div(void);
void labs(void);
void ldiv(void);
void llabs(void);
void lldiv(void);


/*Multibyte characters
 * leave that unimplemented for now

mblen
    Get length of multibyte character (function )

mbtowc
    Convert multibyte sequence to wide character (function )

wctomb
    Convert wide character to multibyte sequence (function )


Multibyte strings

mbstowcs
    Convert multibyte string to wide-character string (function )

wcstombs
    Convert wide-character string to multibyte string (function )*/


#endif // STDLIB_H
