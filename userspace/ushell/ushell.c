#include <stdio.h>

int main(void)
{
    puts("ULMIX shell\n\n $ ");

    char c;
    for (;;)
    {
        c = getchar();
        if (c == EOF) break;
        putchar(c);
        if (c == '\n')
            puts("$ ");
    }

    return 0;
}

