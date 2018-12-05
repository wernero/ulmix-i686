#include "path.h"
#include "util/util.h"
#include "filesystem/filesystem.h"

extern fnode_t root;

static fnode_t *get_working_dir(void);
static fnode_t *get_child_by_name(fnode_t *parent, char *name);
static fnode_t *find_node(fnode_t *working_dir, char *rel_path);

fnode_t *get_node(const char *path)
{
    char *tmp = (char*)path;
    if (path[0] == '/')
    {
        if (path[1] == 0)
            return &root;
        return find_node(&root, tmp+1);
    }

    return find_node(get_working_dir(), tmp);
}

static fnode_t *find_node(fnode_t *working_dir, char *rel_path)
{
    char next_name[64];

    for (int i = 0; i < strlen(rel_path); i++)
    {
        if (rel_path[i] == '/')
        {
            next_name[i] = 0;
            if (strlen(next_name) == 0)
            {
                return NULL;
            }

            fnode_t *next = get_child_by_name(working_dir, next_name);
            if (next == NULL || next->meta.type != FILE_DIRECTORY)
            {
                return NULL;
            }

            return find_node(next, rel_path + i + 1);
        }

        next_name[i] = rel_path[i];
    }

    return get_child_by_name(working_dir, next_name);
}

static fnode_t *get_child_by_name(fnode_t *parent, char *name)
{
    fnode_t *child;
    for (child = parent->children; child != NULL; child = child->next)
    {
        if (strcmp(child->meta.name, name) == 0)
            return child;
    }
    return NULL;
}

static fnode_t *get_working_dir(void)
{
    return NULL;
}
