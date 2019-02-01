#include <libgen.h>
#include <string.h>

char *basename(char *pathname)
{
    unsigned long ind = strlen(pathname);
    while (--pathname[ind])
    {
        if (pathname[ind] == '/')
            return pathname + ind + 1;
    }

    return pathname;
}
