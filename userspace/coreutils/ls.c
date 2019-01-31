#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define F_ALL       0x01
#define F_ALMOST    0x02
#define F_LONG      0x04

int ls(int argc, char *argv[])
{
    char *pathname = ".";
    unsigned long flags = F_LONG;


    DIR *directory;
    directory = opendir(pathname);
    if (directory == NULL)
    {
        printf("%s: error: %s\n", argv[0], strerror(errno));
        return 1;
    }

    struct dirent *fil;
    while ((fil = readdir(directory)) != NULL)
    {
        if (flags & F_LONG)
        {
            char type = 'd';
            char *up = "rwx";
            char *gp = "rwx";
            char *op = "rwx";
            char *owner = "root";
            char *group = owner;
            unsigned long size = 4096;

            printf("%c"         // file type
                   "%s%s%s "    // user, group, others (permissions)
                   "%s %s "     // owner, group
                   "%08lu "     // size
                   "%s\n",      // file name
                   type, up, gp, op,
                   owner, group,
                   size, fil->d_name);
        }
        else
        {
            printf("%s\n", fil->d_name);
        }
    }

    closedir(directory);
}
