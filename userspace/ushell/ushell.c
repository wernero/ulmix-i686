#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void getline(char *buf)
{
    char c;
    while ((c = getchar()) != '\n')
    {
        if (c == EOF)
            break;
        *buf++ = c;
        putchar(c);
    }
    putchar('\n');
    *buf = 0;
}

int main(void)
{
    printf("ULMIX shell\n\n");

    char buf[PATH_MAX];
    while (1)
    {
        if (getwd(buf) == NULL)
            printf("cannot obtain pathname\n");

        printf("%s$ ", buf);

        getline(buf);
        //if (strcmp(buf, "exit") == 0)
        //    break;

        if (chdir(buf) < 0)
            printf("path not found\n");
    }

    printf("\n\nexit\n");
    return 0;
}

