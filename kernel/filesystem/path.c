#include "path.h"
#include "filesystem.h"
#include <sched/task.h>
#include <sched/process.h>
#include <errno.h>
#include <kdebug.h>

extern struct dir_struct root;
extern thread_t *current_thread;

static char *strccpy(char *dest, char *src, char terminator);

static int namei_recursive(char *path, struct dir_struct *working_dir, struct direntry_struct **node)
{
    char cname[20];
    char *rem = strccpy(cname, path, '/');  // buffer overflow??
    if (strlen(cname) == 0)
        return -ENOENT;

    if (working_dir->entries == NULL)
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

                *node = entry;

                klog(KLOG_DEBUG, "namei(): '%s' -> inode #%d",
                     cname,
                     (*node)->inode_no);
                return SUCCESS;
            }

            if (entry->type != DIRECTORY)
                return -ENOENT;

            if (entry->directory->entries == NULL && entry->type == DIRECTORY)
            {
                // get directory data not cached directory
                direntry_get_dir(entry->directory);
            }

            return namei_recursive(rem, entry->directory, node);
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
    if (c == 0)
        src--;
    *dest = 0;
    return src;
}

