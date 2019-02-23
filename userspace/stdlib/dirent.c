#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>

extern int actual_errno;

DIR *opendir(char *name)
{
    int fd = open(name, O_RDONLY | O_DIRECTORY);
    if (fd < 0)
        return NULL;

    DIR *dirp = malloc(sizeof(DIR));
    dirp->fileno = fd;
    return dirp;
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
