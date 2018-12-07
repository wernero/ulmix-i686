#ifndef PARTITIONS_H
#define PARTITIONS_H

#include "drivers/devices.h"
#include "filesystem/vfscore.h"
#include "util/util.h"

typedef struct _mbr_entry mbr_entry_t;
struct _mbr_entry
{
    int bootable;           // bootable = 1
    int is_lba48;           // uses LBA48 addressing (for more than 2TB of addressable space)
    uint8_t system_id;      // Filesystem ID
    uint32_t start_sector;  // offset in sectors
    uint32_t sector_count;  // total size
    int id;                 // partition #
};

int install_disk(fd_t *device);

#endif // PARTITIONS_H
