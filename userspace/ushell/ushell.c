#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/kdebug.h>

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
    //char *path = "/bin/";
    char *execve_argv[ARG_MAX];

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

        if (strcmp(argvp[0], "kheap") == 0)
        {
            kheapdump();
            continue;
        }

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

        int i, detached = 0;
        for (i = 0; i < argc; i++)
        {
            if (i == argc - 1 && strcmp(argvp[i], "&") == 0)
            {
                detached = 1;
                continue;
            }
            execve_argv[i] = argvp[i];
        }
        execve_argv[i] = NULL;

        pid_t child;
        if ((child = fork()) == 0)
        {
            if (execve(command, execve_argv, NULL) < 0)
            {
                printf("error: %s: %s\n", command, strerror(errno));
                exit(1);
            }
        }

        if (!detached)
            waitpid(child, NULL, 0);
    }

    printf("\n\nexit\n");
    return 0;
}

