#include <stdio.h>
#include <string.h>
#include <libgen.h>

extern int ls(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    if (argc < 1)
        return 1;

    char prg[32];
    strcpy(prg, argv[0]);
    char *base = basename(prg);

    if (strcmp(base, "coreutils") == 0)
    {
        printf("Ulmix coreutils\n"
               "Copyright (C) 2019\n"
               "Written by Alexander Ulmer\n");
        return 0;
    }

    if (strcmp(base, "ls") == 0)
    {
        return ls(argc, argv);
    }

    printf("coreutils: %s: command not found\n", base);
    return 1;
}
