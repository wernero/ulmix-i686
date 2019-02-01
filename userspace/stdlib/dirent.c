#include <dirent.h>
#include <errno.h>
#include <stdlib.h>

DIR *opendir(char *name)
{
    return NULL;
}

int closedir(DIR *dirp)
{
    return -ENOSYS;
}

struct dirent* readdir(DIR *dirp)
{
    return NULL;
}
