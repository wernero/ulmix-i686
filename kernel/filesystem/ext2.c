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

static int ext2_probe(struct file_struct *fd);
static int ext2_mount(struct filesystem_struct *fs, struct dir_struct *mountpoint, struct file_struct *fd);
static int ext2_get_direntry(struct dir_struct *miss);
static int ext2_get_inode(struct direntry_struct *entry);

static ssize_t ext2_read(struct file_struct *fd, char *buf, size_t len);
static ssize_t ext2_write(struct file_struct *fd, char *buf, size_t len);
static ssize_t ext2_seek(struct file_struct *fd, size_t offset, int whence);


void install_ext2()
{
    struct filesystem_struct *ext2fs = kmalloc(sizeof(struct filesystem_struct), 1, "filesystem_struct");

    ext2fs->fs_probe = ext2_probe;
    ext2fs->fs_mount = ext2_mount;
    ext2fs->fs_get_direntry = ext2_get_direntry;
    ext2fs->fs_get_inode = ext2_get_inode;
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

    klog(KLOG_DEBUG, "ext2_probe(): signature=0x%x", signature);

    if (signature == 0xef53)
        return 0;
    return -1;
}

static int ext2_mount(struct filesystem_struct *fs, struct dir_struct *mountpoint, struct file_struct *fd)
{
    struct gendisk_struct *bd = fd->drv.bd;
    mountpoint->bd = bd;
    // mountpoint->part possibly not used

    // get superblock from partition
    superblock_extended_t *superblock = kmalloc(sizeof(superblock_extended_t), 1, "superblock_extended_t");
    get_superblock(fd, superblock);

    klog(KLOG_DEBUG, "ext2_mount(): signature=%x, total_inodes=%d, total_blocks=%d, unalloc_inodes=%d, unalloc_blocks=%d",
        superblock->signature,
        superblock->total_inodes,
        superblock->total_blocks,
        superblock->unalloc_inodes,
        superblock->unalloc_blocks);

    // initalize sb_struct in dir_struct
    mountpoint->sb = kmalloc(sizeof(struct sb_struct), 1 , "sb_struct");
    mountpoint->sb->fd = fd;
    mountpoint->sb->fs = fs;
    mountpoint->sb->s_blocks_total = superblock->total_blocks;
    mountpoint->sb->s_inodes_total = superblock->total_inodes;
    mountpoint->sb->s_blocks_per_group = superblock->blocks_per_group;
    mountpoint->sb->s_inodes_per_group = superblock->inodes_per_group;

    mountpoint->sb->s_gdb_count = superblock->total_blocks / superblock->blocks_per_group + 1; // **TODO** correct round up

    kfree(superblock);

    //  struct hd_struct *partition = &(bd->part_list[part]);
    char *group_descriptor_buf;
    blockgroup_descriptor_t group_descriptor;
    int group_descriptor_offset = 0x800;
    int group_descriptor_size = 0x0;
    struct gd_struct * current_gds;

    group_descriptor_size = (((mountpoint->sb->s_gdb_count * sizeof(blockgroup_descriptor_t)) / 0x200) + 1) * 0x200;
//    group_descriptor_size = 0x400;

    group_descriptor_buf = kmalloc(group_descriptor_size, 1, "group_descriptor_buf");

    fd->fops.seek(fd, group_descriptor_offset, SEEK_SET);
    fd->fops.read(fd, (char*)group_descriptor_buf, group_descriptor_size);

    klog(KLOG_DEBUG, "ext2_mount(): gdb=%x, gdc=%x, gds=%x, gdo=%x",
        group_descriptor_buf,
        mountpoint->sb->s_gdb_count,
        group_descriptor_size,
        mountpoint->partition->sector_offset + (group_descriptor_offset / 512)
        );

    mountpoint->sb->s_group_desc = kmalloc(sizeof(struct gd_struct),1,"gd_struct");
    current_gds = mountpoint->sb->s_group_desc;


    for(int i = 0; i < mountpoint->sb->s_gdb_count; i++) {

        memcpy(&group_descriptor,(group_descriptor_buf + i * 0x20), 0x20);

        klog(KLOG_DEBUG, "ext2_mount(): bba=%x, iba=%x, ita=%x, ub=%d, ui=%d, dc=%d",
            group_descriptor.block_bitmap_addr,
            group_descriptor.inode_bitmap_addr,
            group_descriptor.inode_table_addr,
            group_descriptor.unalloc_blocks,
            group_descriptor.unalloc_inodes,
            group_descriptor.dir_count
            );

        current_gds->bg_block_bitmap = group_descriptor.block_bitmap_addr;          /* Blocks bitmap block */
        current_gds->bg_inode_bitmap = group_descriptor.inode_bitmap_addr;          /* Inodes bitmap block */
        current_gds->bg_inode_table = group_descriptor.inode_table_addr;            /* Inodes table block */
        current_gds->bg_free_blocks_count = group_descriptor.unalloc_blocks;        /* Free blocks count */
        current_gds->bg_free_inodes_count = group_descriptor.unalloc_inodes;        /* Free inodes count */
        current_gds->bg_used_dirs_count = group_descriptor.dir_count;               /* Directories count */
        current_gds->bg_next = 0x0;

        if(i < (mountpoint->sb->s_gdb_count-1)) { // no kmalloc if last element

            current_gds->bg_next = kmalloc(sizeof(struct gd_struct),1,"gd_struct");
            current_gds = current_gds->bg_next;
        }

    }

    mountpoint->inode_no = 2; // root inode of a partition is always 2


    ext2_get_direntry(mountpoint); // get directories entries for


    // for now
    return 0;
}

static int ext2_get_direntry(struct dir_struct *miss)
{
    // TODO

    klog(KLOG_DEBUG, "ext2_get_direntry(): miss=%x",
        miss
        );


    int block_group = 0;
    struct gd_struct * gds;
    int i;

    int inode_group_offset = 0;

    char *inode_buf = kmalloc(0x200, 1, "inode_buf");
    char *direntry_buf = kmalloc(0x400, 1, "direntry_buf");
    ext2_inode_t *inode = kmalloc(sizeof(ext2_inode_t), 1, "ext2_inode_t");

    ext2_direntry2_t *de = kmalloc(sizeof(ext2_direntry2_t), 1, "ext2_direntry2_t");

    struct direntry_struct *current_des;

    if(miss->entries == NULL) {
        // dir_struct has no entries; create a first new one
        miss->entries = kmalloc(sizeof(struct direntry_struct),1,"direntry_struct");
        current_des = miss->entries;

        klog(KLOG_DEBUG, "ext2_get_direntry(): new dirstruct current_des=%x",
            current_des
            );

    //TODO

    } else {
        // there are entries present_find the last one and update list
        while(miss->entries != NULL) {
            // no entries for direntry_struct in dir_struct

        //TODO

        }
    }


    // which inode do we want to read
    // which block group is required - inode / inode per block
    block_group =  miss->inode_no / miss->sb->s_inodes_per_group;

    gds = miss->sb->s_group_desc; // this is group descriptor 0;

    for(i=1; i <= block_group; i++){
        gds = gds->bg_next;
    }

    klog(KLOG_DEBUG, "ext2_get_direntry(): gds0=%x, gds=%x, blkgrp=%x, ita=%x, ita_off=%x",
        miss->sb->s_group_desc,
        gds,
        block_group,
        gds->bg_inode_table,
        gds->bg_inode_table * EXT2_BLOCK_SIZE
        );

    // there are 4 inodes structs (0x80) (inode_groups) per disk block (0x200) - calculate the offset of the inode address table beginning

    inode_group_offset = (((miss->inode_no - 1) % miss->sb->s_inodes_per_group) / 4);

    // get inode address table from respective block and jump to it
    miss->sb->fd->fops.seek(miss->sb->fd, (gds->bg_inode_table * EXT2_BLOCK_SIZE) + (inode_group_offset * 512), SEEK_SET);
    miss->sb->fd->fops.read(miss->sb->fd, (char*)inode_buf, 512);


    // get respective inode data

    memcpy(inode,(inode_buf + (((miss->inode_no - 1) % 4) * 0x80)), 0x80);

    // check if directory 0x4000
    // klog(KLOG_DEBUG, "ext2_get_direntry(): mode=%x, size=%x, data0=%x, data1=%x",
    //     inode->i_mode,
    //     inode->i_size,
    //     inode->i_block[0] * EXT2_BLOCK_SIZE,
    //     inode->i_block[1] * EXT2_BLOCK_SIZE
    //     );

    // get directory data0
    miss->sb->fd->fops.seek(miss->sb->fd, inode->i_block[0] * EXT2_BLOCK_SIZE, SEEK_SET);
    miss->sb->fd->fops.read(miss->sb->fd, (char*)direntry_buf, EXT2_BLOCK_SIZE);

    for(i=0; i < 0x400;){
        memcpy(&de->inode, direntry_buf+i, sizeof(uint32_t));
        i += sizeof(uint32_t);
        memcpy(&de->rec_len, direntry_buf+i, sizeof(uint16_t));
        i += sizeof(uint16_t);
        memcpy(&de->name_len, direntry_buf+i, sizeof(uint8_t));
        i += sizeof(uint8_t);
        memcpy(&de->file_type, direntry_buf+i, sizeof(uint8_t));
        i += sizeof(uint8_t);

        memset(&de->name ,0, EXT2_NAME_LEN);
//        memcpy(de->name, miss->name, strlen(miss->name));
//        memcpy(de->name + strlen(miss->name), direntry_buf+i, de->name_len);
        memcpy(de->name, direntry_buf+i, de->name_len);
        i += (de->rec_len - 8);

        klog(KLOG_DEBUG, "ext2_get_direntry(): i=%d, inode=%d, rec_len=%x, name_len=%x, file_type=%x, name=%s",
            i,
            de->inode,
            de->rec_len,
            de->name_len,
            de->file_type,
            de->name
            );

        memcpy(current_des->name, de->name, de->name_len);
        current_des->inode_no = de->inode;

        current_des->parent = miss;
        current_des->directory = NULL;
        current_des->next = kmalloc(sizeof(struct direntry_struct),1,"direntry_struct");

        ext2_get_inode(current_des);

        if(current_des->mode & 0x4000) {   // inode is a directory entry
            //current_des->type = DIRECTORY;
            current_des->directory->parent = miss;

            current_des->read_opens = 0;
            current_des->write_opens = 0;

            //current_des->fd = NULL;

            if(current_des->inode_no == miss->inode_no) {
                current_des->directory = miss;
            } else {

                current_des->directory = kmalloc(sizeof(struct dir_struct),1,"dir_struct");

                current_des->directory->mountpoint = 0; // not a mountpoint so far

                current_des->directory->sb = miss->sb;
                current_des->directory->bd = miss->bd;
                current_des->directory->partition = miss->partition;

                // adding parent name is probably not a good idea ..
//                memcpy(current_des->directory->name, miss->name, strlen(miss->name));
//                memcpy(current_des->directory->name + strlen(miss->name), current_des->name, strlen(current_des->name));
                memcpy(current_des->directory->name, current_des->name, strlen(current_des->name));

                 // needs to be changed sprintf("%s%s/",miss->name, current_des->name);
                current_des->directory->inode_no = current_des->inode_no;

                current_des->directory->entries = NULL;

                klog(KLOG_DEBUG, "ext2_get_direntry(): new dir_struct added %x, name=%s",
                    current_des->directory,
                    current_des->directory->name
                    );

            }

        } else if(current_des->mode & 0x8000) {   // inode is a file entry
            //current_des->type = REGULAR;
            current_des->read_opens = 0;
            current_des->write_opens = 0;

            current_des->directory = NULL;
        } else {
            //current_des->type = UNKOWN;
            current_des->read_opens = 1;
            current_des->write_opens = 1;
        }

        current_des = current_des->next;
    }


    return 0;
}

static int fetch_inode(struct direntry_struct *entry, ext2_inode_t *inode)
{
    if (entry->inode_no > entry->parent->sb->s_inodes_total)
        return -ENOENT;

    // get group descriptor for the block group that contains the inode
    struct gd_struct *gds = entry->parent->sb->s_group_desc;
    int block_group = (entry->inode_no - 1) / entry->parent->sb->s_inodes_per_group;
    for(int i = 0; i < block_group; i++)
        gds = gds->bg_next;

    // calculate inode offset inside the group
    int inodes_per_disk_sector = IO_SIZE / EXT2_INODE_SIZE;
    int inode_group_offset = ((entry->inode_no - 1) % entry->parent->sb->s_inodes_per_group) / inodes_per_disk_sector;

    // read inode from disk
    char *inode_buf = kmalloc(IO_SIZE, 1, "ext2 inode_buf");

    entry->parent->sb->fd->fops.seek(entry->parent->sb->fd,
                                     (inode_group_offset * 512) + (gds->bg_inode_table * EXT2_BLOCK_SIZE),
                                     SEEK_SET);
    entry->parent->sb->fd->fops.read(entry->parent->sb->fd, (char*)inode_buf, 512);

    memcpy(inode, inode_buf + (((entry->inode_no - 1) % inodes_per_disk_sector) * EXT2_INODE_SIZE), EXT2_INODE_SIZE);
    kfree(inode_buf);
    return SUCCESS;
}

static int ext2_get_inode(struct direntry_struct *entry)
{
    int ret;
    ext2_inode_t *inode = kmalloc(sizeof(ext2_inode_t), 1, "ext2_inode_t");
    if ((ret = fetch_inode(entry, inode)) < 0)
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
        entry->parent->sb->fd->fops.seek(entry->parent->sb->fd, inode->i_block[n] * EXT2_BLOCK_SIZE, SEEK_SET);
        entry->parent->sb->fd->fops.read(entry->parent->sb->fd, (char*)inode_indirect_buf, EXT2_BLOCK_SIZE);

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
        entry->parent->sb->fd->fops.seek(entry->parent->sb->fd,
                                         inode->i_block[n] * EXT2_BLOCK_SIZE,
                                         SEEK_SET);
        entry->parent->sb->fd->fops.read(entry->parent->sb->fd, (char*)inode_indirect_buf, EXT2_BLOCK_SIZE);

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

            entry->parent->sb->fd->fops.seek(entry->parent->sb->fd,
                                             inode_indirect_buf->i_indirect_ptr[ibc] * EXT2_BLOCK_SIZE,
                                             SEEK_SET);
            entry->parent->sb->fd->fops.read(entry->parent->sb->fd, (char*)inode_dindirect_buf, EXT2_BLOCK_SIZE);

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
                fd->direntry->parent->sb->fd->fops.seek(fd->direntry->parent->sb->fd, lba * 512, SEEK_SET);
                fd->direntry->parent->sb->fd->fops.read(fd->direntry->parent->sb->fd, disk_read_buffer, EXT2_BLOCK_SIZE);

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

