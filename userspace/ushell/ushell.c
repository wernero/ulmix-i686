#include <stdio.h>

static void readline(char *buf)
{
    char c;
    while ((c = getchar()) != '\n')
        *(buf++) = c;
    *buf = 0;
}

int main(void)
{
    puts("ULMIX shell\n");

    char buf[256];
    readline(buf);

    puts("have read the following line: \n");
    puts(buf);


    return 0;
}

