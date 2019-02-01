#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define ARG_LENGTH  64
#define ARG_MAX     16
char argvp[ARG_LENGTH][ARG_MAX];

int getline(char argv[ARG_LENGTH][ARG_MAX])
{
    int argc = 0;
    int argc_ind = 0;

    char c;
    while ((c = getchar()) != EOF)
    {
        putchar(c);

        if (c == ' ' || c == '\n')
        {
            argv[argc][argc_ind] = 0;
            if (argc_ind != 0)
                argc++;
            argc_ind = 0;

            if (c == '\n')
                break;
            continue;
        }

        argv[argc][argc_ind++] = c;
    }
    return argc;
}

int main(void)
{
    printf("ULMIX shell\n\n");

    int argc;
    char buf[64];
    char *homedir = "/";
    //char *execve_param[ARG_MAX];

    while (1)
    {
        if (getwd(buf) == NULL)
            printf("$");
        else
            printf("%s$ ", buf);

        argc = getline(argvp);

        if (argc == 0)
            continue;

        char *command = argvp[0];

        if (strcmp(argvp[0], "exit") == 0)
            return 0;

        if (strcmp(argvp[0], "cd") == 0)
        {
            if (argc < 2)
            {
                if (chdir(homedir) < 0)
                    printf("error: %s\n", strerror(errno));
            }
            else
            {
                if (chdir(argvp[1]) < 0)
                    printf("error: %s\n", strerror(errno));
            }
            continue;
        }

        if (strcmp(command, "pwd") == 0)
        {
            if (getwd(buf) != NULL)
                printf("%s\n", buf);
            else
                printf("error: %s\n", strerror(errno));
            continue;
        }

        printf("error: %s: command not found\n", command);
    }

    printf("\n\nexit\n");
    return 0;
}

