#include "ext2.h"
#include "filesystem.h"
#include "fs_syscalls.h"
#include "vfscore.h"
#include <errno.h>
#include <kdebug.h>
#include <memory/kheap.h>
#include <devices/devices.h>

#define EXT2_BLOCK_SIZE         0x400   // defined by a field in the superblock
#define EXT2_INODE_SIZE         0x80    // defined by a field in the superblock
#define IO_SIZE                 0x200   // actually defined by the actual gendisk_struct

/* ext2 file operations: */
static int ext2_probe(struct file_struct *fd);
static int ext2_mount(struct filesystem_struct *fs, struct dir_struct *mountpoint, struct file_struct *fd);
static int ext2_get_direntries(struct dir_struct *miss);
static ssize_t ext2_read(struct file_struct *fd, char *buf, size_t len);
static ssize_t ext2_write(struct file_struct *fd, char *buf, size_t len);
static ssize_t ext2_seek(struct file_struct *fd, size_t offset, int whence);

void install_ext2()
{
    struct filesystem_struct *ext2fs = kmalloc(sizeof(struct filesystem_struct), 1, "filesystem_struct");

    ext2fs->fs_probe = ext2_probe;
    ext2fs->fs_mount = ext2_mount;
    ext2fs->fs_get_direntries = ext2_get_direntries;
    ext2fs->fs_read = ext2_read;
    ext2fs->fs_write = ext2_write;
    ext2fs->fs_seek = ext2_seek;
    ext2fs->name = "ext2";

    install_fs(ext2fs);
}

static void get_superblock(struct file_struct *fd, superblock_extended_t *buf)
{
    int superblock_offset = 0x400;
    int superblock_size = 0x400;

    fd->fops.seek(fd, superblock_offset, SEEK_SET);
    fd->fops.read(fd, (char*)buf, superblock_size);
}

static int ext2_probe(struct file_struct *fd)
{
    superblock_extended_t *superblock = kmalloc(sizeof(superblock_extended_t), 1, "superblock_extended_t");
    get_superblock(fd, superblock);
    uint16_t signature = superblock->signature;
    kfree(superblock);

    if (signature == 0xef53)
        return SUCCESS;
    return -ENOTSUP;
}

static int ext2_mount(struct filesystem_struct *fs, struct dir_struct *mountpoint, struct file_struct *fd)
{
    // store mount info
    struct mntp_struct *mnt_info = kmalloc(sizeof(struct mntp_struct), 1, "mnt_info");
    mnt_info->bd = fd->drv.bd;
    mnt_info->fs = fs;
    mountpoint->mnt_info = mnt_info;
    mountpoint->mountpoint = mountpoint;

    // get superblock from partition
    superblock_extended_t *superblock = kmalloc(sizeof(superblock_extended_t), 1, "superblock_extended_t");
    get_superblock(fd, superblock);

    // initalize sb_struct superblock data in mnt_info
    mnt_info->sb = kmalloc(sizeof(struct sb_struct), 1 , "sb_struct");
    struct sb_struct *sb = mnt_info->sb;
    sb->fd = fd;    // file descriptor used to access block device
    sb->fs = fs;    // ext2 calls to be called by the user
    sb->s_blocks_total = superblock->total_blocks;
    sb->s_inodes_total = superblock->total_inodes;
    sb->block_size = 0x400 << superblock->block_size_log;
    sb->s_blocks_per_group = superblock->blocks_per_group;
    sb->s_inodes_per_group = superblock->inodes_per_group;
    sb->s_gdb_count = superblock->total_blocks / superblock->blocks_per_group + 1; // **TODO** correct round up
    kfree(superblock);

    // obtain the group descriptor table
    size_t bgroup_table_offset = 2 * sb->block_size;
    if (sb->block_size != 0x400)
        bgroup_table_offset = 1 * sb->block_size;
    size_t bgroup_table_size = (((sb->s_gdb_count * sizeof(struct gd_struct)) / 0x200) + 1) * IO_SIZE;

    sb->group_descriptors = kmalloc(bgroup_table_size, 1, "bgroup descriptors");
    fd->fops.seek(fd, bgroup_table_offset, SEEK_SET);
    fd->fops.read(fd, (char*)sb->group_descriptors, bgroup_table_size);

    mountpoint->inode_no = 2;       // has to be set for ext2_get_direntry() to work
    ext2_get_direntries(mountpoint);  // cache root directory

    klog(KLOG_INFO, "ext2: mounted file system (%d/%d)", mnt_info->bd->major, mnt_info->bd->minor);
    return SUCCESS;
}

static int fetch_inode(unsigned int inode_no, struct mntp_struct *mnt_info, ext2_inode_t *inode)
{
    struct sb_struct *entry_sb = mnt_info->sb;
    if (inode_no > entry_sb->s_inodes_total)
        return -ENOENT;

    // get group descriptor for the block group that contains the inode
    int block_group_no = (inode_no - 1) / entry_sb->s_inodes_per_group;
    struct gd_struct *block_group = &(entry_sb->group_descriptors[block_group_no]);

    // calculate inode offset inside the group
    int inodes_per_disk_sector = IO_SIZE / EXT2_INODE_SIZE;
    int inode_group_offset = ((inode_no - 1) % entry_sb->s_inodes_per_group) / inodes_per_disk_sector;

    // read inode from disk
    char *inode_buf = kmalloc(IO_SIZE, 1, "ext2 inode_buf");

    entry_sb->fd->fops.seek(entry_sb->fd,
                                     (block_group->bg_inode_table * EXT2_BLOCK_SIZE) + (inode_group_offset * IO_SIZE),
                                     SEEK_SET);
    entry_sb->fd->fops.read(entry_sb->fd, (char*)inode_buf, IO_SIZE);

    memcpy(inode, inode_buf + (((inode_no - 1) % inodes_per_disk_sector) * EXT2_INODE_SIZE), EXT2_INODE_SIZE);
    kfree(inode_buf);
    klog(KLOG_DEBUG, "fetching inode #%d, size=0x%x (~%S)", inode_no, inode->i_size, inode->i_size);
    return SUCCESS;
}

static int ext2_get_inode(struct direntry_struct *entry)
{
    struct sb_struct *entry_sb = entry->parent->mnt_info->sb;

    int ret;
    ext2_inode_t *inode = kmalloc(sizeof(ext2_inode_t), 1, "ext2_inode_t");
    if ((ret = fetch_inode(entry->inode_no, entry->parent->mnt_info, inode)) < 0)
    {
        kfree(inode);
        return -ret;
    }

    int n;
    int block_counter = 0;

    struct inode_block_table *current_ibt;
    entry->mode = inode->i_mode;
    entry->type = entry->mode & 0xf000;
    entry->size = inode->i_size;
    entry->size_blocks = inode->i_blocks;
    entry->bptr1 = inode->i_block[0];


    if(inode->i_block[0] != NULL) {

        entry->blocks = kmalloc(sizeof(struct inode_block_table), 1, "inode_block_table");

        current_ibt = entry->blocks;                	// used as rolling pointer to current struct
        current_ibt->file = entry;                  	// this is the file we are workig on
        current_ibt->next = NULL;

    for(n=0; n < VFS_INODE_BLOCK_TABLE_LEN; n++)	// init all blocks to zero
        current_ibt->blocks[n] = 0;

    ext2_inode_blk_table_t *inode_indirect_buf;

        for(n=0; n < EXT2_N_BLOCKS; n++) {

            if (inode->i_block[n] == 0)
          break;  					// not an block (or pointer to block)

            if (n < EXT2_NDIR_BLOCKS) {         	// handling of direct blocks

                klog(KLOG_DEBUG, "ext2_get_inode(): inode=%d, blocks=%x",
                    entry->inode_no,
                    inode->i_block[n]
                    );

                current_ibt->blocks[block_counter % VFS_INODE_BLOCK_TABLE_LEN] = inode->i_block[n];
        block_counter += 1;

            }
            switch(n){

          case EXT2_IND_BLOCK:
                klog(KLOG_DEBUG, "ext2_get_inode(): indirect inode=%d, blocks=%x",
                    entry->inode_no,
                    inode->i_block[n]
                    );

        inode_indirect_buf = kmalloc(sizeof(ext2_inode_blk_table_t), 1, "inode_indirect_buf");

        // block is pointing to 1k data block which holds max 256 block pointers to 1k data blocks .. 256k filessize
        entry_sb->fd->fops.seek(entry_sb->fd, inode->i_block[n] * EXT2_BLOCK_SIZE, SEEK_SET);
        entry_sb->fd->fops.read(entry_sb->fd, (char*)inode_indirect_buf, EXT2_BLOCK_SIZE);

        for(int ibc = 0; ibc < EXT2_IND_BLOCK_LEN; ibc++) {
            if(inode_indirect_buf->i_indirect_ptr[ibc] == 0)
              break;

            // check if new ibt needs to be created
            if(block_counter == VFS_INODE_BLOCK_TABLE_LEN) {
            // create new block
            current_ibt->next = kmalloc(sizeof(struct inode_block_table), 1, "inode_block_table");
            current_ibt->next->file = current_ibt->file;
            current_ibt = current_ibt->next;
            }

            // store block pointer
            current_ibt->blocks[block_counter % VFS_INODE_BLOCK_TABLE_LEN] = inode_indirect_buf->i_indirect_ptr[ibc];
            block_counter += 1;
        } // for ibc

        kfree(inode_indirect_buf); // be nice and free up space
        break;
          case EXT2_DIND_BLOCK:
                klog(KLOG_DEBUG, "ext2_get_inode(): double inode=%d, blocks=%x",
                    entry->inode_no,
                    inode->i_block[n]
                    );

        inode_indirect_buf = kmalloc(sizeof(ext2_inode_blk_table_t), 1, "single inode_indirect_buf");

        // block is pointing to 1k data block which holds max 256x256 block pointers  of 1k data blocks .. 65MB filesize
        entry_sb->fd->fops.seek(entry_sb->fd,
                                         inode->i_block[n] * EXT2_BLOCK_SIZE,
                                         SEEK_SET);
        entry_sb->fd->fops.read(entry_sb->fd, (char*)inode_indirect_buf, EXT2_BLOCK_SIZE);

        for(int ibc = 0; ibc < EXT2_IND_BLOCK_LEN; ibc++) {
            if(inode_indirect_buf->i_indirect_ptr[ibc] == 0)
              break;

            ext2_inode_blk_table_t *inode_dindirect_buf = kmalloc(sizeof(ext2_inode_blk_table_t), 1, "double inode_indirect_buf");

            // check if new ibt needs to be created
            if(block_counter == VFS_INODE_BLOCK_TABLE_LEN) {
            // create new block
            current_ibt->next = kmalloc(sizeof(struct inode_block_table), 1, "inode_block_table");
            current_ibt->next->file = current_ibt->file;
            current_ibt = current_ibt->next;
            }

            entry_sb->fd->fops.seek(entry_sb->fd,
                                             inode_indirect_buf->i_indirect_ptr[ibc] * EXT2_BLOCK_SIZE,
                                             SEEK_SET);
            entry_sb->fd->fops.read(entry_sb->fd, (char*)inode_dindirect_buf, EXT2_BLOCK_SIZE);

            for(int dibc = 0; dibc < EXT2_IND_BLOCK_LEN; dibc++) {

            if(inode_dindirect_buf->i_indirect_ptr[dibc] == 0)
              break;

            // store block pointer
            current_ibt->blocks[block_counter % VFS_INODE_BLOCK_TABLE_LEN] = inode_dindirect_buf->i_indirect_ptr[dibc];
            block_counter += 1;

            } // for dibc
            kfree(inode_dindirect_buf); // be nice and free up space
        } // for ibc

        kfree(inode_indirect_buf); // be nice and free up space
        break;
          case EXT2_TIND_BLOCK:
                klog(KLOG_DEBUG, "ext2_get_inode(): triple inode=%d, blocks=%x",
                    entry->inode_no,
                    inode->i_block[n]
                    );

          //TODO -- handling of triple indirect files ..  15GB filesize ...

        break;
          default:
        break;
        } // case
        }
    }

    klog(KLOG_DEBUG, "ext2_get_inode(): **total** inode=%d, block_counter=%d",
      entry->inode_no,
      block_counter
      );

    entry->payload = (void *) inode;

    return 0;
}

static int ext2_get_direntries(struct dir_struct *miss)
{
    struct sb_struct *sb = miss->mnt_info->sb;

    if (miss->entries != NULL)
    {
        // maybe update the files? for now, not supported
        klog(KLOG_WARN, "ext2: warn: fetching directory that already contains files");
        return -ENOTSUP;
    }

    ext2_inode_t *inode = kmalloc(sizeof(ext2_inode_t), 1, "ext2 inode buf");
    int ret = fetch_inode(miss->inode_no, miss->mnt_info, inode);
    if (ret < 0)
        return ret;

    // get block containing the file list
    char *direntry_buf = kmalloc(EXT2_BLOCK_SIZE, 1, "ext2 direntry buf");
    sb->fd->fops.seek(sb->fd, inode->i_block[0] * EXT2_BLOCK_SIZE, SEEK_SET);
    sb->fd->fops.read(sb->fd, (char*)direntry_buf, EXT2_BLOCK_SIZE);

    struct ext2_direntry_struct raw_entry;
    struct direntry_struct *current_entry = kmalloc(sizeof(struct direntry_struct), 1, "direntry_struct");
    miss->entries = current_entry;

    for (int i = 0; i < inode->i_size; ) // TODO: read more blocks!!!
    {
        // get entry
        memcpy(&raw_entry, direntry_buf + i, sizeof(struct ext2_direntry_struct));
        memcpy(current_entry->name, direntry_buf + sizeof(struct ext2_direntry_struct) + i, raw_entry.name_len);
        i += raw_entry.rec_len;
        i += (i % 4) ? 4 - (i % 4) : 0;

        current_entry->name[raw_entry.name_len] = 0;
        current_entry->inode_no = raw_entry.inode;
        current_entry->parent = miss;
        current_entry->directory = NULL;

        klog(KLOG_DEBUG, "direntry: inode=%d, fil_type=%d, name='%s'",
             current_entry->inode_no,
             raw_entry.file_type,
             current_entry->name);

        ext2_get_inode(current_entry);

        if (current_entry->mode & DIRECTORY)
        {
            if (current_entry->inode_no == miss->inode_no)
            {
                current_entry->directory = miss;
            }
            else
            {
                current_entry->directory = kmalloc(sizeof(struct dir_struct), 1, "dir_struct");
                current_entry->directory->parent = miss;
                current_entry->directory->mountpoint = miss->mountpoint;
                current_entry->directory->mnt_info = miss->mnt_info;
                current_entry->directory->entries = NULL;
                current_entry->directory->inode_no = current_entry->inode_no;
                // name?
            }
        }

        current_entry->next = kmalloc(sizeof(struct direntry_struct), 1, "direntry_struct");
        current_entry = current_entry->next;
    }

    return 0;
}

static ssize_t ext2_read(struct file_struct *fd, char *buf, size_t len)
{
    size_t bytes_read = 0;
    size_t total_bytes_read = 0;
    struct direntry_struct *entry = fd->direntry;

    if(entry == NULL) // something is really wrong
        return -EIO;

    if(entry->blocks == NULL) // inode has no blocks to read from
      return -EIO;

    // do not overrun file length
    size_t bytes_to_copy;
    size_t seek_offset = fd->seek_offset;
    if(len > (entry->size - seek_offset))
        bytes_to_copy = entry->size - seek_offset;
    else
        bytes_to_copy = len;

    if (bytes_to_copy == 0)
        goto read_done;

    size_t start_block = seek_offset / EXT2_BLOCK_SIZE;
    size_t start_block_offset = seek_offset % EXT2_BLOCK_SIZE;

    char *disk_read_buffer = kmalloc(EXT2_BLOCK_SIZE, 1, "ext2_read() disk_read_buffer");
    struct inode_block_table *current_ibt;
    size_t lba_fac, lba;
    for (current_ibt = entry->blocks; bytes_to_copy > 0; current_ibt = current_ibt->next)
    {
        if (current_ibt == NULL)
        {
            // something really bad happend (e.g. size wrong)
            kfree(disk_read_buffer);
            return -EIO;
        }

        for (int i = 0; i < VFS_INODE_BLOCK_TABLE_LEN; i++)
        {
            if (current_ibt->blocks[i] == 0)
                break;

            lba_fac = EXT2_BLOCK_SIZE / 512;
            lba = current_ibt->blocks[i] * lba_fac;

            if (!start_block) // depends on seek_offset
            {
                struct sb_struct *sb = fd->direntry->parent->mnt_info->sb;
                sb->fd->fops.seek(sb->fd, lba * 512, SEEK_SET);
                sb->fd->fops.read(sb->fd, disk_read_buffer, EXT2_BLOCK_SIZE);

                if (bytes_to_copy < EXT2_BLOCK_SIZE)
                {
                    memcpy(buf + bytes_read, disk_read_buffer + start_block_offset, bytes_to_copy);
                    total_bytes_read += bytes_to_copy;
                    bytes_to_copy = 0;
                    break; // nothing else to copy
                }
                else
                {
                    memcpy(buf + bytes_read, disk_read_buffer + start_block_offset, EXT2_BLOCK_SIZE);
                    bytes_to_copy -= EXT2_BLOCK_SIZE;
                    total_bytes_read += EXT2_BLOCK_SIZE;
                }

                bytes_read += EXT2_BLOCK_SIZE - start_block_offset;

                start_block_offset = 0; // only to be done on the very first block read;
            }

            if (start_block != 0)
                start_block--;
        }
    }

    kfree(disk_read_buffer);

read_done:
    klog(KLOG_DEBUG, "ext2_read(): inode=%d, size=%S, bytes: requested=%d : read=%d",
         entry->inode_no,
         entry->size,
         len,
         total_bytes_read);

    return total_bytes_read;
}

static ssize_t ext2_write(struct file_struct *fd, char *buf, size_t len)
{
    // not implemented yet
    return -ENOSYS;
}

static ssize_t ext2_seek(struct file_struct *fd, size_t offset, int whence)
{
    switch(whence)
    {
    case SEEK_SET:
        fd->seek_offset = offset;
        break;
    case SEEK_CUR:
        fd->seek_offset += offset;
        break;
    case SEEK_END:
        fd->seek_offset = fd->direntry->size - 1;
        break;
    default:
        return -EINVAL;
    }

    return fd->seek_offset;
}

