#ifndef PARTITIONS_H
#define PARTITIONS_H

#include "devices.h"

struct mbr_entry_struct
{
    int bootable;           // bootable = 1
    int is_lba48;           // uses LBA48 addressing (for more than 2TB of addressable space)
    uint8_t system_id;      // Filesystem ID
    uint32_t start_sector;  // offset in sectors
    uint32_t sector_count;  // total size
    int id;                 // partition #
};

int part_scan(struct gendisk_struct *bd);

#endif // PARTITIONS_H
