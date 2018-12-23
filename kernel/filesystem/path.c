#include "path.h"
#include <sched/task.h>
#include <filesystem/filesystem.h>
#include <errno.h>
#include "log.h"

extern struct dir_struct root;
extern thread_t *current_thread;

static char *strccpy(char *dest, char *src, char terminator);

static int namei_recursive(char *path, struct dir_struct *working_dir, struct direntry_struct **node)
{
    char cname[256];
    char *rem = strccpy(cname, path, '/');  // buffer overflow??
    if (strlen(cname) == 0)
        return -ENOENT;


    klog(KLOG_INFO, "namei_recursive(): ********** cname=%s rem=%s",
        cname,
        rem
        );

    struct direntry_struct *entry;
    for (entry = working_dir->entries; entry != NULL; entry = entry->next)
    {

        klog(KLOG_INFO, "namei_recursive(): name=%s type=%x, inode=%d, dir=%x, sb=%x, fs=%x",
            entry->name,
            entry->type,
            entry->inode_no,
            entry->directory,
            entry->parent->sb,
            entry->parent->sb->fs
        );

        if (strcmp(entry->name, cname) == 0)
        {
            if (*rem == 0)
            {
                if (*(rem - 1) == '/' && entry->type != DIRECTORY)
                    return -ENOTDIR;

//                if (entry->payload == NULL)  // payload is the raw inode data
//                    direntry_get_inode(entry);

                *node = entry;
                return 0;
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
    *dest = 0;
    return src;
}
