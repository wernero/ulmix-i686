#include "path.h"
#include "filesystem.h"
#include <sched/task.h>
#include <sched/process.h>
#include <errno.h>
#include <kdebug.h>

extern struct dir_struct root;
extern thread_t *current_thread;

static void reverse_path(char *buf, size_t size)
{
    char tmp[size];
    strcpy(tmp, buf);
    *buf = 0;

    for (int i = strlen(tmp) - 1;; i--)
    {
        if (i == 0)
        {
            strcat(buf, tmp);
            break;
        }

        if (tmp[i] == '/')
        {
            strcat(buf, tmp + i + 1);
            strcat(buf, "/");
            tmp[i] = 0;
        }
    }
}

int sys_getcwd(char *buf, size_t size)
{
    if (size < 1)
        return -EINVAL;

    if (current_thread->process->working_dir == &root)
    {
        strcpy(buf, "/");
        return SUCCESS;
    }

    struct direntry_struct *node = current_thread->process->working_dir->entry;
    if (node == NULL)
        return -ENOENT;

    *buf = 0;
    strcat(buf, node->name);
    strcat(buf, "/");

    struct dir_struct *parent = node->parent;
    while (1)
    {
       if (parent == &root)
           break;

       strcat(buf, parent->entry->name);
       strcat(buf, "/");

       parent = parent->parent;
    }

    reverse_path(buf, size);
    return SUCCESS;
}

static char *strccpy(char *dest, char *src, char terminator);
static int namei_recursive(char *path, struct dir_struct *working_dir, struct direntry_struct **node)
{
    char cname[256];
    char *rem = strccpy(cname, path, '/');
    if (strlen(cname) == 0)
    {
        strcpy(cname, ".");
    }

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

