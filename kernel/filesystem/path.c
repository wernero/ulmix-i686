#include "path.h"
#include <sched/task.h>
#include <filesystem/filesystem.h>
#include <errno.h>

extern struct dir_struct root;
extern thread_t *current_thread;

static char *strccpy(char *dest, char *src, char terminator);

static int namei_recursive(char *path, struct dir_struct *working_dir, struct direntry_struct **node)
{
    char cname[256];
    char *rem = strccpy(cname, path, '/');
    if (strlen(cname) == 0)
        return -ENOENT;

    struct direntry_struct *entry;
    for (entry = working_dir->entries; entry != NULL; entry = entry->next)
    {
        if (strcmp(entry->name, cname) == 0)
        {
            if (*rem == 0)
            {
                if (*(rem - 1) == '/' && entry->type != DIRECTORY)
                    return -ENOTDIR;

                if (entry->payload == NULL)
                    direntry_get_inode(entry);

                *node = entry;
                return 0;
            }

            if (entry->type != DIRECTORY)
                return -ENOENT;
            return namei_recursive(rem, (struct dir_struct *)entry->payload, node);
        }
    }

    return -ENOENT;
}

int namei(char *path, struct direntry_struct **node)
{
    struct dir_struct *working_dir = current_thread->process->working_dir;
    if (path[0] == '/')
    {
        path++;
        working_dir = &root;
    }

    return namei_recursive(path, working_dir, node);
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
