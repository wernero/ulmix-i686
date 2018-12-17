#include "path.h"
#include <sched/task.h>

extern struct dir_struct root;
extern thread_t *current_thread;

static char *strccpy(char *dest, char *src, char terminator);

int namei(char *path, struct direntry_struct *node)
{
    struct dir_struct *working_dir = current_thread->process->working_dir;
    if (path[0] == '/')
    {
        path++;
        working_dir = &root;
    }

    char cname[256];
    char *rem = strccpy(cname, path, '/');
    if (strlen(cname) == 0)
        return -1;

    struct direntry_struct *entry;
    for (entry = working_dir->entries; entry != NULL; entry = entry->next)
    {
        if (strcmp(entry->name, cname) == 0)
        {
            if (*rem == 0)
            {
                if (*(rem - 1) == '/' && entry->type != DIRECTORY)
                    return -1;

                node = entry;
                return 0;
            }

            return namei(rem, node);
        }
    }

    return -1;
}

static char* strccpy(char *dest, char *src, char terminator)
{
    char c;
    while ((c = *(src++)))
    {
        if (c == terminator)
            break;
        *(dest++) = c;
    }
    *dest = 0;
    return src;
}
