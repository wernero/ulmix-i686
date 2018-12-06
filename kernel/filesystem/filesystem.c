#include "filesystem.h"
#include "memory/kheap.h"
#include "log.h"
#include "filesystem/ext2.h"

fnode_t root;
#define SUP_FS_COUNT 10
filesystem_t *supported_filesystems[SUP_FS_COUNT];

int install_fs(filesystem_t *fs)
{
    for (int i = 0; i < SUP_FS_COUNT; i++)
    {
        if (supported_filesystems[i] == NULL)
        {
            supported_filesystems[i] = fs;
            return 0;
        }
    }
    return -1;
}

void vfs_init()
{
    for (int i = 0; i < SUP_FS_COUNT; i++)
        supported_filesystems[i] = NULL;

    install_ext2();

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

static int is_mbr(unsigned char *sec1)
{
    if (sec1[0x1fe] == 0x55 && sec1[0x1ff] == 0xAA)
    {
        return 1;
    }

    return 0;
}

int part_mount(fnode_t *mount_point, file_t *device, mbr_entry_t partition)
{
    klog(KLOG_DEBUG, "part_mount(): mounting partition: start=0x%x, size=%S, bootable=%d, fs_type=%s",
         partition.start_sector * 512, partition.sector_count * 512, partition.bootable,
         (partition.system_id == 0x83) ? "linux-native" : "unknown");

    for (int i = 0; i < SUP_FS_COUNT; i++)
    {
        filesystem_t *pfs = supported_filesystems[i];
        if (pfs != NULL && (pfs->fs_probe(device, partition) >= 0))
        {
            klog(KLOG_DEBUG, "part_mount(): partition is of type %s", pfs->name);

            return pfs->fs_mount(mount_point, device, partition);
        }
    }
    return -1;
}

int disk_mount(fnode_t *mount_point, file_t *device)
{
    if (mount_point == NULL)
    {
        klog(KLOG_WARN, "disk_mount(): mount point is NULL");
        return -1;
    }

    if (device == NULL)
    {
        klog(KLOG_WARN, "disk_mount(): device is NULL");
        return -1;
    }

    klog(KLOG_DEBUG, "disk_mount(): mounting device %s", device->name);

    // 1. load Master Boot Record (first sector)
    unsigned char mbr[512];
    device->read(device->drv_struct, (char*)mbr, 1);

    // 2. Verify
    if (!is_mbr(mbr))
    {
        klog(KLOG_DEBUG, "disk_mount(): partition table format not recognized");
        return -1;
    }

    // read MBR partition table
    klog(KLOG_DEBUG, "disk_mount(): reading MBR partition table:");
    for (int i = 0; i < 4; i++)
    {
        mbr_entry_t entry;
        int entry_offset = 0x1be + 16 * i;
        entry.bootable = (mbr[entry_offset] & 0x80) ? 1 : 0;
        entry.is_lba48 = (mbr[entry_offset] & 0x01) ? 1 : 0;

        entry.system_id = mbr[entry_offset + 4];
        entry.start_sector = *((uint32_t*)(mbr + entry_offset + 8));
        entry.sector_count = *((uint32_t*)(mbr + entry_offset + 12));

        if (entry.sector_count > 0 && entry.start_sector > 0)
        {
            part_mount(mount_point, device, entry);

            // for now, only mount 1 partition:
            break;
        }
    }

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



