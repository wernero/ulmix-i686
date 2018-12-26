#ifndef STDIO_H
#define STDIO_H


#define EOF -1
#define NULL 0

typedef struct _file_struct FILE;
struct _file_struct
{

};

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

FILE *fopen(char *filename, char *mode);


void puts(char *s);
void putchar(char c);
char getchar(void);



#endif // STDIO_H
