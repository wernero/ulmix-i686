#include <vfs.h>
#include <errno.h>
#include <string.h>

static struct dir_struct root_dir;
static struct direntry_struct root_direntry;

void __init setup_fs()
{
    root_dir.dir_entry = &root_direntry;
    root_dir.entries = NULL;
    root_dir.mnt_point = NULL;
    root_dir.parent = &root_dir;

    root_direntry.directory = &root_dir;
    root_direntry.type = F_DIR;
    root_direntry.next = NULL;
    root_direntry.parent = &root_dir;
}

void get_direntries(struct dir_struct *dir)
{
    dir->fs_info->fs_info->fops.fs_get_direntries(dir);
}

static const char* strccpy(char *dest, const char *src, char terminator)
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

static int namei_recursive(const char *pathname, struct dir_struct *working_dir, struct direntry_struct **node)
{
    char current_name[256];
    const char *rem = strccpy(current_name, pathname, '/');

    if (strlen(current_name) == 0)
        strcpy(current_name, ".");

    if (working_dir->entries == NULL)
        return -ENOENT;

    struct direntry_struct *entry;
    for (entry = working_dir->entries; entry != NULL; entry = entry->next)
    {
        if (strcmp(entry->name, current_name) == 0)
        {
            if (*rem == 0)
            {
                if (*(rem - 1) == '/' && entry->type != F_DIR)
                    return -ENOTDIR;

                *node = entry;
                return SUCCESS;
            }

            if (entry->type != F_DIR)
                return -ENOENT;

            if (entry->directory->entries == NULL && entry->type == F_DIR)
                get_direntries(entry->directory);
            return namei_recursive(rem, entry->directory, node);
        }
    }

    return -ENOENT;
}

int namei(const char *pathname, struct direntry_struct **node)
{
    // TODO: replace with the actual working dir of the process
    struct dir_struct *working_dir = &root_dir;

    if (pathname[0] == '/')
    {
        // absolute path
        pathname += 1;
        working_dir = &root_dir;

        if (pathname[1] == 0)
        {
            *node = working_dir->dir_entry;
            return SUCCESS;
        }
    }

    return namei_recursive(pathname, working_dir, node);
}
