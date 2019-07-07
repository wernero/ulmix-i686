/*
 * ULMIX ext2 filesystem driver
 * Copyright (C) 2018-2019
 *
 *  Rainer Rudigier <rainer@rudigier.at>
 *  Alexander Ulmer <alexulmer1999@gmail.com>
 *
 * this is the original implementation of the ext2 filesystem
 * from December 2018, where the majority of the code has been
 * written by Rainer Rudigier.
 */

#include <fs.h>
#include <vfs.h>
#include <errno.h>
#include <debug.h>
#include <string.h>
#include <mmap.h>

#define IO_BLOCK_SIZE   512
#define SB_SECT_OFFSET  2
#define SB_SECT_SIZE    2

#define EXT2_NDIR_BLOCKS        12
#define EXT2_IND_BLOCK          EXT2_NDIR_BLOCKS
#define EXT2_DIND_BLOCK         (EXT2_IND_BLOCK + 1)
#define EXT2_TIND_BLOCK         (EXT2_DIND_BLOCK + 1)
#define EXT2_N_BLOCKS           (EXT2_TIND_BLOCK + 1)

typedef struct
{
    // base fields
    uint32_t total_inodes;
    uint32_t total_blocks;
    uint32_t root_blocks;
    uint32_t unalloc_blocks;
    uint32_t unalloc_inodes;
    uint32_t superblock;
    uint32_t block_size_log;
    uint32_t fragment_size_log;
    uint32_t blocks_per_group;
    uint32_t fragments_per_group;
    uint32_t inodes_per_group;
    uint32_t last_mount;
    uint32_t last_write;
    uint16_t mounts_since_check;
    uint16_t mounts_remaining;
    uint16_t signature;
    uint16_t fs_state;
    uint16_t error_action;
    uint16_t minor_version;
    uint32_t last_check;
    uint32_t check_interval;
    uint32_t creator_os;
    uint32_t major_version;
    uint16_t reserved_user;
    uint16_t reserved_group;

    // extended fields
    uint32_t first_inode_non_reserved;
    uint16_t inode_size;
    uint16_t backup_blockgroup;
    uint32_t optional_features;
    uint32_t required_features;
    uint32_t readonly_features;
    char fs_id[16];
    char vol_name[16];
    char last_mount_path[64];
    uint32_t compression_alg;
    uint8_t prealloc_file_blocks;
    uint8_t prealloc_dir_blocks;
    uint16_t unused;
    char journal_id[16];
    uint32_t journal_inode;
    uint32_t journal_device;
    uint32_t orphan_head;
} __attribute__((aligned(1024))) sb_t;

struct inode_struct
{
    uint16_t  i_mode;           /* File mode */
    uint16_t  i_uid;            /* Low 16 bits of Owner Uid */
    uint32_t  i_size;           /* Size in bytes */
    uint32_t  i_atime;          /* Access time */
    uint32_t  i_ctime;          /* Creation time */
    uint32_t  i_mtime;          /* Modification time */
    uint32_t  i_dtime;          /* Deletion Time */
    uint16_t  i_gid;            /* Low 16 bits of Group Id */
    uint16_t  i_links_count;    /* Links count */
    uint32_t  i_blocks;         /* Blocks count */
    uint32_t  i_flags;          /* File flags */

    union {
        struct {
            uint32_t  l_i_reserved1;
        } linux1;
        struct {
            uint32_t  h_i_translator;
        } hurd1;
        struct {
            uint32_t  m_i_reserved1;
        } masix1;
    } osd1;             /* OS dependent 1 */

    uint32_t  i_block[EXT2_N_BLOCKS];/* Pointers to blocks */
    uint32_t  i_generation;   /* File version (for NFS) */
    uint32_t  i_file_acl; /* File ACL */
    uint32_t  i_dir_acl;  /* Directory ACL */
    uint32_t  i_faddr;    /* Fragment address */

    union {
        struct {
            uint8_t    l_i_frag;   /* Fragment number */
            uint8_t    l_i_fsize;  /* Fragment size */
            uint16_t   i_pad1;
            uint16_t  l_i_uid_high;   /* these 2 fields    */
            uint16_t  l_i_gid_high;   /* were reserved2[0] */
            uint32_t   l_i_reserved2;
        } linux2;
        struct {
            uint8_t    h_i_frag;   /* Fragment number */
            uint8_t    h_i_fsize;  /* Fragment size */
            uint16_t  h_i_mode_high;
            uint16_t  h_i_uid_high;
            uint16_t  h_i_gid_high;
            uint32_t  h_i_author;
        } hurd2;
        struct {
            uint8_t    m_i_frag;   /* Fragment number */
            uint8_t    m_i_fsize;  /* Fragment size */
            uint16_t   m_pad1;
            uint32_t   m_i_reserved2[2];
        } masix2;
    } osd2;             /* OS dependent 2 */
} __attribute__((packed));

struct gd_struct
{
    uint32_t bg_block_bitmap;       /* Blocks bitmap block */
    uint32_t bg_inode_bitmap;       /* Inodes bitmap block */
    uint32_t bg_inode_table;        /* Inodes table block */
    uint16_t bg_free_blocks_count;  /* Free blocks count */
    uint16_t bg_free_inodes_count;  /* Free inodes count */
    uint16_t bg_used_dirs_count;    /* Directories count */
} __attribute__((aligned(32)));

struct ext2_direntry_struct
{
    uint32_t  inode;          // inode number
    uint16_t  rec_len;        // directory entry length
    uint8_t   name_len;       // name length
    uint8_t   file_type;      // type or MSB of name length
} __attribute__((packed));

struct ext2fs_struct
{
    sb_t superblock;
    struct hd_struct *part;

    unsigned long block_size;

    // group descriptor table:
    unsigned long gd_count;
    struct gd_struct *group_descriptors;
};

static ssize_t hdd_read(struct ext2fs_struct *fs, unsigned char *buffer,
                        size_t sectors, size_t lba)
{
    return fs->part->bd->fops.read(fs->part->bd->drv_struct,
                     buffer, sectors, lba + fs->part->sect_offset);
}

// commented out as long as not used
/*static ssize_t hdd_write(struct ext2fs_struct *fs, unsigned char *buffer,
                        size_t sectors, size_t lba)
{
    return fs->part->bd->fops.write(fs->part->bd->drv_struct,
                     buffer, sectors, lba + fs->part->sect_offset);
}*/

static int ext2_probe(struct hd_struct *part)
{
    sb_t sb;
    struct ext2fs_struct fs = { .part = part };

    // fetch ext2 superblock from disk
    if (hdd_read(&fs, (unsigned char *)&sb,
            SB_SECT_SIZE, SB_SECT_OFFSET) < SB_SECT_SIZE)
        return -EIO;

    // if the signature matches, we're in
    uint16_t signature = sb.signature;
    if (signature == 0xef53)
        return SUCCESS;

    // otherwise, there's no ext2fs
    return -ENOTSUP;
}

static int fetch_inode(unsigned long inode_no, struct mntp_struct *mnt_info, struct inode_struct *inode)
{
    struct ext2fs_struct *fsinfo = (struct ext2fs_struct*)(mnt_info->fs_data);
    sb_t *sb = &(fsinfo->superblock);

    if (inode_no > sb->total_inodes)
        return -ENOENT;

    // get pointer to group descriptor for the block group that contains the inode
    unsigned block_group_no = (inode_no - 1) / sb->inodes_per_group;
    struct gd_struct *block_group = fsinfo->group_descriptors + block_group_no;

    // calculate the inode offset inside the group
    unsigned inodes_per_disk_sector = IO_BLOCK_SIZE / sb->inode_size;
    unsigned disk_sector_offset = ((inode_no - 1) % sb->inodes_per_group) / inodes_per_disk_sector;
    unsigned inode_table_lba = block_group->bg_inode_table * (fsinfo->block_size / IO_BLOCK_SIZE);

    // fetch it from disk
    int error;
    unsigned char inode_buf[IO_BLOCK_SIZE];
    if ((error = hdd_read(fsinfo, inode_buf, 1,
        inode_table_lba + disk_sector_offset)) < 0)
        return error;

    memcpy(inode, inode_buf + (((inode_no - 1) % inodes_per_disk_sector) * sb->inode_size), sb->inode_size);
    return SUCCESS;
}

static struct inode_struct *direntry_get_inode(struct direntry_struct *direntry)
{
    // allocate memory for the inode if necessary
    struct inode_struct **inode = (struct inode_struct**)&(direntry->payload);
    if (*inode == NULL)
        *inode = kmalloc(sizeof(struct inode_struct), 1, "ext2 inode_struct");

    int error;
    if ((error = fetch_inode(direntry->inode_no, direntry->parent->fs_info, *inode)) < 0)
    {
        kfree(*inode);
        direntry->payload = NULL;
        return NULL;
    }

    return *inode;
}

static int get_direntry(struct direntry_struct *direntry)
{
    ASSERT(direntry != NULL);
    ASSERT(direntry->parent->fs_info != NULL);

    struct inode_struct *inode = direntry_get_inode(direntry);
    if (inode == NULL)
        return -EIO;

    direntry->mode = inode->i_mode;
    direntry->type = direntry->mode & 0xf000;
    direntry->size = inode->i_size;
    direntry->blocks = inode->i_blocks;

    // create block pointer table

    return 0;
}

static int ext2_get_direntries(struct dir_struct *dir)
{
    ASSERT(dir != NULL);
    ASSERT(dir->fs_info != NULL);

    int error;
    struct ext2fs_struct *fsinfo = (struct ext2fs_struct*)dir->fs_info->fs_data;

    if (dir->entries != NULL)
    {
        // maybe update the files? for now, just return
        kprintf("ext2: warning: fetching directory that already contains files\n");
        return SUCCESS;
    }

    // fetch the inode from disk
    struct inode_struct *inode = direntry_get_inode(dir->dir_entry);

    struct ext2_direntry_struct fs_dir_entry;
    struct direntry_struct *current_entry;
    struct direntry_struct **last_next_ptr = &dir->entries;

    int get_new_block = 1;
    long current_block = 0;
    unsigned char direntry_buf[fsinfo->block_size];
    for (int i = 0; i < fsinfo->block_size; ) // TODO: read more blocks
    {
        if (get_new_block)
        {
            get_new_block = 0;
            if ((error = hdd_read(fsinfo, direntry_buf, fsinfo->block_size / IO_BLOCK_SIZE,
                     inode->i_block[current_block++] * (fsinfo->block_size / IO_BLOCK_SIZE))) < 0)
                return error;
        }

        current_entry = kmalloc(sizeof(struct direntry_struct), 1, "direntry_struct");

        // get the actual filesystem specific entry from the buffer
        memcpy(&fs_dir_entry, direntry_buf + i, sizeof(struct ext2_direntry_struct));
        memcpy(current_entry->name, direntry_buf + sizeof(struct ext2_direntry_struct) + i, fs_dir_entry.name_len);
        i += fs_dir_entry.rec_len;
        i += (i % 4) ? 4 - (i % 4) : 0;

        current_entry->name[fs_dir_entry.name_len] = 0;
        current_entry->inode_no = fs_dir_entry.inode;
        current_entry->parent = dir;
        current_entry->directory = NULL;

        if (fs_dir_entry.file_type == 2) // is directory?
        {
            if (current_entry->inode_no == dir->inode_no)
            {
                current_entry->directory = dir;
                current_entry->payload = dir->dir_entry->payload;
            }
            else if (current_entry->inode_no == dir->parent->inode_no)
            {
                current_entry->directory = dir->parent;
                current_entry->payload = dir->parent->dir_entry->payload;
            }
            else
            {
                current_entry->directory = kmalloc(sizeof(struct dir_struct), 1, "dir_struct");
                current_entry->directory->parent = dir;
                current_entry->directory->mnt_point = dir->mnt_point;
                current_entry->directory->fs_info = dir->fs_info;
                current_entry->directory->entries = NULL;
                current_entry->directory->inode_no = current_entry->inode_no;
                current_entry->directory->dir_entry = current_entry;
            }
        }

        ASSERT(get_direntry(current_entry) >= 0);

        *last_next_ptr = current_entry;
        last_next_ptr = &current_entry->next;
    }

    *last_next_ptr = NULL;
    return SUCCESS;
}

static const struct fs_struct ext2fs;
static int ext2_mount(struct hd_struct *part, struct dir_struct *mnt_point)
{
    int error;
    if ((error = ext2_probe(part)) < 0)
        return error;

    // create the data structures describing the file system
    struct ext2fs_struct *fsinfo = kmalloc(sizeof(struct ext2fs_struct), 1, "ext2_info");
    fsinfo->part = part;

    struct mntp_struct *mnt_info = kmalloc(sizeof(struct mntp_struct), 1, "mntp_struct");
    mnt_info->part = part;
    mnt_info->fs_info = &ext2fs;
    mnt_info->fs_data = fsinfo;

    // fetch the superblock from disk
    if ((error = hdd_read(fsinfo, (unsigned char*)&fsinfo->superblock,
                 SB_SECT_SIZE, SB_SECT_OFFSET)) < SB_SECT_SIZE)
        goto no_mount;

    // initialize some variables
    fsinfo->block_size = 0x400 << fsinfo->superblock.block_size_log;
    fsinfo->gd_count = fsinfo->superblock.total_blocks
            / fsinfo->superblock.blocks_per_group + 1;

    // locate the group descriptor table and fetch it into memory
    unsigned gdt_lba = (fsinfo->block_size == 0x400) ? 2 : 1;
    gdt_lba *= fsinfo->block_size / IO_BLOCK_SIZE;
    unsigned gdt_size = fsinfo->gd_count * sizeof(struct gd_struct);
    unsigned gdt_size_sectors = gdt_size / IO_BLOCK_SIZE;
    if (gdt_size % IO_BLOCK_SIZE != 0)
        gdt_size_sectors += 1;

    fsinfo->group_descriptors = kmalloc(gdt_size_sectors * IO_BLOCK_SIZE,
                                           1, "ext2 gd_struct[]");
    if ((error = hdd_read(fsinfo, (unsigned char*)fsinfo->group_descriptors,
                          gdt_size_sectors, gdt_lba)) < 0)
        goto no_mount_free_gdt;

    // define the directory as mount point and give it the
    // information on where to find filesystem functions
    struct dir_struct *mnt_point_backup = mnt_point->mnt_point;
    struct mntp_struct *fs_info_backup = mnt_point->fs_info;
    mnt_point->fs_info = mnt_info;
    mnt_point->mnt_point = mnt_point;

    // fetch the root inode and it's immediate children
    mnt_point->inode_no = 2;
    mnt_point->dir_entry->inode_no = 2;
    if ((error = ext2_get_direntries(mnt_point)) < 0)
        goto no_mount_unmount;

    return SUCCESS;

no_mount_unmount:
    mnt_point->fs_info = fs_info_backup;
    mnt_point->mnt_point = mnt_point_backup;
no_mount_free_gdt:
    kfree(fsinfo->group_descriptors);
no_mount:
    kfree(fsinfo);
    kfree(mnt_info);
    return error;
}

static const struct fs_struct ext2fs = {
    .name = "ext2",
    .mbr_id = 0x83,
    .fops = {
        .fs_probe = ext2_probe,
        .fs_mount = ext2_mount,
        .fs_get_direntries = ext2_get_direntries
    }
};

void __init init_ext2()
{
    register_fs(&ext2fs);
}

