#include "vfscore.h"
#include "log.h"
#include "memory/kheap.h"
#include "filesystem/filesystem.h"

fnode_t root;

static int insert_child_node(fnode_t *parent, fnode_t *child);

void vfs_init()
{
    // load built-in file system drivers
    init_filesystems();

    // create file system root
    file_t froot =
    {
        .type = FILE_DIRECTORY,
        .permissions = PERM_USER_READ | PERM_USER_WRITE | PERM_USER_EXEC
                        | PERM_GRP_READ | PERM_GRP_EXEC
                        | PERM_OTH_READ | PERM_OTH_EXEC,
        .io_size = 0,
        .drv_struct = NULL,
        .group = 0,
        .owner = 0,
        .read = NULL,
        .write = NULL,
        .seek = NULL,
        .name[0] = 0
    };
    root.meta = froot;
    root.parent = NULL;
    root.next = NULL;
    root.previous = NULL;
    root.children = NULL;

    // directory /dev -> mount point for the devices
    if (mkdir(&root, "dev") == NULL)
    {
        klog(KLOG_DEBUG, "failed to create dir /dev");
    }
}

static int insert_child_node(fnode_t *parent, fnode_t *child)
{
    if (parent == NULL || child == NULL)
        return -1;

    child->parent = parent;

    if (parent->children == NULL)
    {
        parent->children = child;
        child->next = child->previous = NULL;
    }
    else
    {
        child->next = parent->children->next;
        child->previous = parent->children;
        parent->children->next = child;
    }

    return 0;
}

fnode_t *mkdir(fnode_t *directory, char *name)
{
    file_t fdir =
    {
        .type = FILE_DIRECTORY,
        .permissions = PERM_USER_READ | PERM_USER_WRITE | PERM_USER_EXEC
                        | PERM_GRP_READ | PERM_GRP_EXEC
                        | PERM_OTH_READ | PERM_OTH_EXEC,
        .io_size = 0,
        .drv_struct = NULL,
        .group = 0,
        .owner = 0,
        .read = NULL,
        .write = NULL,
        .seek = NULL,
    };
    strcpy(fdir.name, name);
    return mknod(directory, fdir);
}

fnode_t *mknod(fnode_t *directory, file_t file)
{
    if (directory == NULL)
    {
        return NULL;
    }

    fnode_t *node = kmalloc(sizeof(fnode_t), 1, "fnode_t");
    node->meta = file;
    node->children = NULL;
    insert_child_node(directory, node);
    return node;
}
