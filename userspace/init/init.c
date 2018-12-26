#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void getline(char *buf);
static void run(char *filename);

/*
 * TEST SHELL
 * possible commands:
 *
 * exit
 * <program>
 */

int main()
{
    puts("*** ulmix test shell ***\n\n");

    char buf[30];
    while (1)
    {
        puts("$ ");
        getline(buf);

        if (strcmp(buf, "exit") == 0)
            break;
        else
            run(buf);
    }

    puts("exit\n");
    return 0;
}

static void run(char *filename)
{
    pid_t pid = fork();
    if (pid)
    {
        if (execve(filename, NULL, NULL) < 0)
        {
            puts("error: command not found\n");
        }
    }
    else
    {
        // wait until child program terminates
        // waitpid()
    }
}

static void getline(char *buf)
{
    char c;
    while ((c = getchar()) != '\n')
    {
        *buf++ = c;
    }
    *buf = 0;
}
