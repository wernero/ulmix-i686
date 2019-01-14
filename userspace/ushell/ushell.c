#include <stdio.h>

static void readline(char *buf)
{
    char c = getchar();
    while (c != '\n' && c != EOF)
    {
        *(buf++) = c;
        c = getchar();
    }
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

