#include <unistd.h>
#include <stdlib.h>

char *getwd(char *buf)
{
    return getcwd(buf, PATH_MAX);
}

char *get_current_dir_name(void)
{
    char *path = malloc(PATH_MAX);
    return getcwd(path, PATH_MAX);
}
