#include <stdio.h>

int main(void)
{
    printf("ULMIX shell\n\n");

    int x = 58;
    printf("x=%d, hex=%x\n", x, x);

    char *hw = "Hello, world!";
    printf("hw=%s\n", hw);

    printf("$ ");

    char c;
    while ((c = getchar()) != '\n')
    {
        if (c == EOF)
            break;
        putchar(c);
    }

    printf("\n\nexit\n");
    return 0;
}

