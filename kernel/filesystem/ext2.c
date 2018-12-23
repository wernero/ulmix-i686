#include "ext2.h"
#include "memory/kheap.h"
#include "log.h"
#include "drivers/devices.h"
#include "filesystem/filesystem.h"
#include "filesystem/fs_syscalls.h"
#include "filesystem/vfscore.h"

#define EXT2_BLOCK_SIZE         0x400

static int ext2_probe(struct gendisk_struct *bd, int partition);
static int ext2_mount(struct filesystem_struct *fs, struct dir_struct *mountpoint, struct gendisk_struct *bd, int part);
static int ext2_get_direntry(struct dir_struct *miss);
static int ext2_get_inode(struct direntry_struct *entry, unsigned long inode_no);


void install_ext2()
{
    struct filesystem_struct *ext2fs = kmalloc(sizeof(struct filesystem_struct), 1, "filesystem_struct");

    ext2fs->fs_probe = ext2_probe;
    ext2fs->fs_mount = ext2_mount;
    ext2fs->fs_get_direntry = ext2_get_direntry;
    ext2fs->fs_get_inode = ext2_get_inode;
    ext2fs->name = "ext2";

    install_fs(ext2fs);
}

static void get_superblock(struct gendisk_struct *bd, struct hd_struct *partition, superblock_extended_t *buf)
{
    int superblock_offset = 0x400;
    int superblock_size = 0x400;

    bd->fops.seek(bd->drv_struct, partition->sector_offset + (superblock_offset / 512), SEEK_SET);
    bd->fops.read(bd->drv_struct, (char*)buf, superblock_size / 512);
}

static int ext2_probe(struct gendisk_struct *bd, int partition)
{
    superblock_extended_t *superblock = kmalloc(sizeof(superblock_extended_t), 1, "superblock_extended_t");
    get_superblock(bd, &(bd->part_list[partition]), superblock);
    uint16_t signature = superblock->signature;
    kfree(superblock);

    klog(KLOG_INFO, "ext2_probe(): signature=%x", signature);

    if (signature == 0xef53)
        return 0;
    return -1;
}

static int ext2_mount(struct filesystem_struct *fs, struct dir_struct *mountpoint, struct gendisk_struct *bd, int part)
{
    mountpoint->bd = bd;
    mountpoint->partition = &(bd->part_list[part]);

    // get superblock from partition
    superblock_extended_t *superblock = kmalloc(sizeof(superblock_extended_t), 1, "superblock_extended_t");
    get_superblock(bd, mountpoint->partition, superblock);

    klog(KLOG_INFO, "ext2_mount(): signature=%x, total_inodes=%d, total_blocks=%d, unalloc_inodes=%d, unalloc_blocks=%d",
        superblock->signature,
        superblock->total_inodes,
        superblock->total_blocks,
        superblock->unalloc_inodes,
        superblock->unalloc_blocks);

    // initalize sb_struct in dir_struct
    mountpoint->sb = kmalloc(sizeof(struct sb_struct), 1 , "sb_struct");

    mountpoint->sb->fs = fs; // to fix function calls to ext2 FS functions which
                             // are installed via install_fs .. (filesystem.c)
    mountpoint->sb->s_blocks_total = superblock->total_blocks;
    mountpoint->sb->s_inodes_total = superblock->total_inodes;
    mountpoint->sb->s_blocks_per_group = superblock->blocks_per_group;
    mountpoint->sb->s_inodes_per_group = superblock->inodes_per_group;

    // how many blocks do we have
    mountpoint->sb->s_gdb_count = superblock->total_blocks / superblock->blocks_per_group + 1; // **TODO** correct rouund up

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

    bd->fops.seek(bd->drv_struct, mountpoint->partition->sector_offset + (group_descriptor_offset / 512), SEEK_SET);
    bd->fops.read(bd->drv_struct, (char*)group_descriptor_buf, group_descriptor_size / 512);


    klog(KLOG_INFO, "ext2_mount(): gdb=%x, gdc=%x, gds=%x, gdo=%x",
        group_descriptor_buf,
        mountpoint->sb->s_gdb_count,
        group_descriptor_size,
        mountpoint->partition->sector_offset + (group_descriptor_offset / 512)
        );

    mountpoint->sb->s_group_desc = kmalloc(sizeof(struct gd_struct),1,"gd_struct");
    current_gds = mountpoint->sb->s_group_desc;


    for(int i = 0; i < mountpoint->sb->s_gdb_count; i++) {

        memcpy(&group_descriptor,(group_descriptor_buf + i * 0x20), 0x20);

        klog(KLOG_INFO, "ext2_mount(): bba=%x, iba=%x, ita=%x, ub=%d, ui=%d, dc=%d",
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

    klog(KLOG_INFO, "ext2_get_direntry(): miss=%x",
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

        klog(KLOG_INFO, "ext2_get_direntry(): new dirstruct current_des=%x",
            current_des
            );



    } else {
        // there are entries present_find the last one and update list
        while(miss->entries != NULL) {
            // no entries for direntry_struct in dir_struct

        }


    }


    // which inode do we want to read
    // which block group is required - inode / inode per block
    block_group =  miss->inode_no / miss->sb->s_inodes_per_group;

    gds = miss->sb->s_group_desc; // this is group descriptor 0;

    for(i=1; i <= block_group; i++){
        gds = gds->bg_next;
    }

    klog(KLOG_INFO, "ext2_get_direntry(): gds0=%x, gds=%x, blkgrp=%x, ita=%x, ita_off=%x",
        miss->sb->s_group_desc,
        gds,
        block_group,
        gds->bg_inode_table,
        gds->bg_inode_table * EXT2_BLOCK_SIZE
        );

    // there are 4 inodes structs (0x80) (inode_groups) per disk block (0x200) - calculate the offset of the inode address table beginning

    inode_group_offset = (((miss->inode_no - 1) % miss->sb->s_inodes_per_group) / 4);

    // get inode address table from respective block and jump to it
    miss->bd->fops.seek(miss->bd->drv_struct, miss->partition->sector_offset + (gds->bg_inode_table * EXT2_BLOCK_SIZE / 512) + inode_group_offset, SEEK_SET);
    miss->bd->fops.read(miss->bd->drv_struct, (char*)inode_buf, 0x200 / 512);

    // get respective inode data

    memcpy(inode,(inode_buf + (((miss->inode_no - 1) % 4) * 0x80)), 0x80);

    // check if directory 0x4000
    // klog(KLOG_INFO, "ext2_get_direntry(): mode=%x, size=%x, data0=%x, data1=%x",
    //     inode->i_mode,
    //     inode->i_size,
    //     inode->i_block[0] * EXT2_BLOCK_SIZE,
    //     inode->i_block[1] * EXT2_BLOCK_SIZE
    //     );

    // get directory data0
    miss->bd->fops.seek(miss->bd->drv_struct, miss->partition->sector_offset + (inode->i_block[0] * EXT2_BLOCK_SIZE / 512), SEEK_SET);
    miss->bd->fops.read(miss->bd->drv_struct, (char*)direntry_buf, 0x400 / 512);

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

        klog(KLOG_INFO, "ext2_get_direntry(): i=%d, inode=%d, rec_len=%x, name_len=%x, file_type=%x, name=%s",
            i,
            de->inode,
            de->rec_len,
            de->name_len,
            de->file_type,
            de->name
            );



        memcpy(current_des->name, de->name, de->name_len);
        current_des->inode_no = de->inode;
        //current_des->payload;

        current_des->parent = miss;
        current_des->directory = NULL;
        current_des->next = kmalloc(sizeof(struct direntry_struct),1,"direntry_struct");

        ext2_get_inode(current_des, current_des->inode_no);


        if(current_des->mode & 0x4000) {   // inode is a directory entry
            current_des->type = DIRECTORY;
            current_des->directory->parent = miss;

            current_des->read_opens = 0;
            current_des->write_opens = 0;


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

                klog(KLOG_INFO, "ext2_get_direntry(): new dir_struct added %x, name=%s",
                    current_des->directory,
                    current_des->directory->name
                    );

            }

        } else if(current_des->mode & 0x8000) {   // inode is a file entry
            current_des->type = REGULAR;
            current_des->read_opens = 0;
            current_des->write_opens = 0;

            current_des->directory = NULL;
        } else {
            current_des->type = UNKOWN;
            current_des->read_opens = 1;
            current_des->write_opens = 1;
        }

        current_des = current_des->next;
    }


    return 0;
}

static int ext2_get_inode(struct direntry_struct *entry, unsigned long inode_no)
{

    struct gd_struct * gds;
    int block_group = 0;
    int inode_group_offset = 0;
    //int inode_offset = 0;
    int i,n;
    int block_counter = 0;

    char *inode_buf = kmalloc(0x200, 1, "inode_buf");
    ext2_inode_t *inode = kmalloc(sizeof(ext2_inode_t), 1, "ext2_inode_t");

    struct inode_block_table *current_ibt;

    // which inode do we want to read
    // which block group is required - inode / inode per block
    block_group =  (entry->inode_no - 1) / entry->parent->sb->s_inodes_per_group;

    gds = entry->parent->sb->s_group_desc; // this is group descriptor 0;

    for(i=1; i <= block_group; i++){
        gds = gds->bg_next;
    }

    // there are 4 inodes structs (0x80) (inode_groups) per disk block (0x200) - calculate the offset of the inode address table beginning

    inode_group_offset = (((entry->inode_no - 1) % entry->parent->sb->s_inodes_per_group) / 4);

    klog(KLOG_INFO, "ext2_get_inode(): bg=%d, inode=%d, offset=%d, name=%s, bg_inode_table=%x, part_off=%d, read_at=%d : %x",
        block_group,
        entry->inode_no,
        inode_group_offset,
        entry->name,
        gds->bg_inode_table,
        entry->parent->partition->sector_offset,
        entry->parent->partition->sector_offset + (gds->bg_inode_table * EXT2_BLOCK_SIZE / 512) + inode_group_offset,
        (entry->parent->partition->sector_offset + (gds->bg_inode_table * EXT2_BLOCK_SIZE / 512) + inode_group_offset)*0x200
        );

    // get inode address table from respective block and jump to it
    entry->parent->bd->fops.seek(entry->parent->bd->drv_struct, entry->parent->partition->sector_offset + (gds->bg_inode_table * EXT2_BLOCK_SIZE / 512) + inode_group_offset, SEEK_SET);
    entry->parent->bd->fops.read(entry->parent->bd->drv_struct, (char*)inode_buf, 0x200 / 512);


    memcpy(inode,(inode_buf + (((entry->inode_no - 1) % 4) * 0x80)), 0x80);


    klog(KLOG_INFO, "ext2_get_inode(): inode=%d, mode=%x, size=%d",
        entry->inode_no,
        inode->i_mode,
        inode->i_size
        );

    entry->mode = inode->i_mode;


    if(inode->i_block[0] != NULL) {

        entry->blocks = kmalloc(sizeof(struct inode_block_table), 1, "inode_block_table");

        current_ibt = entry->blocks;                // used as rolling pointer to current struct
        current_ibt->file = entry;                  // this is the file we are workig on

        for(n=0; n < EXT2_N_BLOCKS; n++) {

            if (inode->i_block[n] == 0) break;  // not an block (or pointer to block)


            if (n < EXT2_NDIR_BLOCKS) {         // handling of direct blocks

                klog(KLOG_INFO, "ext2_get_inode(): inode=%d, blocks=%x",
                    entry->inode_no,
                    inode->i_block[n]
                    );

                current_ibt->blocks[block_counter % VFS_INODE_BLOCK_TABLE_LEN] = inode->i_block[n];

            }
            // handling of indirect / double / triple block refrences // TODO

            block_counter += 1;

        }
    }

    entry->payload = (void *) inode;

    // TODO
    return 0;
}
