#include "filesystem.h"
#include "memory/kheap.h"
#include "log.h"

fnode_t root;

void vfs_init()
{
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

    // make some directories:
    if (mkdir(&root, "dev") == NULL)
    {
        klog(KLOG_DEBUG, "failed to create dir /dev");
    }
}

int mount(fnode_t *mount_point, file_t *device)
{
    klog(KLOG_DEBUG, "mount() TEST");
    if (mount_point == NULL)
    {
        klog(KLOG_DEBUG, "mount(): mount point is NULL");
        return -1;
    }

    if (device == NULL)
    {
        klog(KLOG_DEBUG, "mount(): device is NULL");
        return -1;
    }

    // load 4096 bytes from Hard disk -> 8 sectors
    unsigned char buf[4096];
    device->read(device->drv_struct, (char *)buf, 8);

    unsigned char sig = buf[0x638];
    klog(KLOG_DEBUG, "mount(): byte 1 of signature is %x", sig);
    sig = buf[0x639];
    klog(KLOG_DEBUG, "mount(): byte 2 of signature is %x", sig);

    cli();
    hlt();
    return 0;
}

static void insert_node(fnode_t *node, fnode_t *subnode)
{
    if (node == NULL)
    {
        klog(KLOG_WARN, "insert_node(): parent node is NULL");
        return;
    }
    if (subnode == NULL)
    {
        klog(KLOG_WARN, "insert_node(): subnode is NULL");
        return;
    }

    subnode->parent = node;

    if (node->children == NULL)
    {
        node->children = subnode;
        subnode->next = subnode->previous = NULL;
    }
    else
    {
        subnode->next = node->children->next;
        subnode->previous = node->children;
        node->children->next = subnode;
    }
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
        klog(KLOG_WARN, "mknod(): parent directory is NULL");
        return NULL;
    }

    fnode_t *node = kmalloc(sizeof(fnode_t), 1, "fnode_t");
    node->meta = file;
    node->children = NULL;
    insert_node(directory, node);
    return node;
}



