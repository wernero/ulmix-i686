#include "partitions.h"
#include "log.h"

static void gendisk_add_partition(struct gendisk_struct *bd, struct mbr_entry_struct mbr_entry);
static int is_mbr(unsigned char *sec1);

int part_scan(struct gendisk_struct *bd)
{
    klog(KLOG_DEBUG, "part_scan()");

    // load master boot record
    unsigned char mbr[512];
    if (bd->fops.read(bd->drv_struct, (char*)mbr, 1) < 0)
    {
        return -1;
    }

    // verify master boot record
    if (!is_mbr(mbr))
    {
        klog(KLOG_DEBUG, "part_scan(): no valid MBR found");
        return -1;
    }

    // read MBR partition table
    klog(KLOG_DEBUG, "part_scan(): reading MBR partition table:");

    int partitions_count = 0;
    for (int i = 0; i < 4; i++)
    {
        struct mbr_entry_struct entry;
        int entry_offset = 0x1be + 16 * i;
        entry.bootable = (mbr[entry_offset] & 0x80) ? 1 : 0;
        entry.is_lba48 = (mbr[entry_offset] & 0x01) ? 1 : 0;

        entry.id = i;
        entry.system_id = mbr[entry_offset + 4];
        entry.start_sector = *((uint32_t*)(mbr + entry_offset + 8));
        entry.sector_count = *((uint32_t*)(mbr + entry_offset + 12));

        if (entry.sector_count > 0 && entry.start_sector > 0)
        {
            // found new partition
            gendisk_add_partition(bd, entry);
            partitions_count++;
        }
    }

    return partitions_count;
}

static void gendisk_add_partition(struct gendisk_struct *bd, struct mbr_entry_struct mbr_entry)
{
    struct hd_struct partition;
    partition.sector_offset = mbr_entry.start_sector;
    partition.sector_count = mbr_entry.sector_count;
    partition.fs_type = mbr_entry.system_id;
    itoa(mbr_entry.id, partition.name);

    bd->part_list[mbr_entry.id] = partition;
    bd->part_count++;

    klog(KLOG_DEBUG, "part_scan(): part #%d: size=%S, type=%d", mbr_entry.id, partition.sector_count * 512, partition.fs_type);
}

static int is_mbr(unsigned char *sec1)
{
    if (sec1[0x1fe] == 0x55 && sec1[0x1ff] == 0xAA)
    {
        return 1;
    }

    return 0;
}
