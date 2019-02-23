#include <dirent.h>
#include <errno.h>
#include <stdlib.h>

extern int actual_errno;

DIR *opendir(char *name)
{
    actual_errno = ENOSYS;
    return NULL;
}

int closedir(DIR *dirp)
{
    actual_errno = ENOSYS;
    return -1;
}

struct dirent* readdir(DIR *dirp)
{
    actual_errno = ENOSYS;
    return NULL;
}
