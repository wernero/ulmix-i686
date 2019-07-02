#include <fs.h>
#include <types.h>
#include <debug.h>

struct mbr_entry_struct
{
    int bootable;
    int is_lba48;
    uint8_t fs_id;
    uint32_t start_sector;
    uint32_t sector_count;
    int id;
};

static int is_mbr(unsigned char *sector)
{
    if (sector[0x1fe] == 0x55
        && sector[0x1ff] == 0xAA)
        return 1;
    return 0;
}

static void bd_add_part(struct gendisk_struct *bd, struct mbr_entry_struct entry)
{
    // add partition to block device
    bd->part_count += 1;
    struct hd_struct *part = &(bd->part_list[entry.id]);

    part->fs_type = entry.fs_id;
    part->sect_count = entry.sector_count;
    part->sect_offset = entry.start_sector;
    part->bd = bd;
}

void gendisk_partscan(struct gendisk_struct *bd)
{
    unsigned char mbr[512];
    if (bd->fops.read(bd->drv_struct, mbr, 1, 0) < 1)
        return;

    if (!is_mbr(mbr))
        return;

    unsigned entry_offset;
    struct mbr_entry_struct entry;
    for (int i = 0; i < 4; i++)
    {
        entry_offset = 0x1be + (16 * i);

        entry.id = i;
        entry.bootable = (mbr[entry_offset] & BIT(7)) ? 1 : 0;
        entry.is_lba48 = (mbr[entry_offset] & BIT(0)) ? 1 : 0;
        entry.fs_id = mbr[entry_offset + 4];
        entry.start_sector = *((uint32_t*)(mbr + entry_offset + 8));
        entry.sector_count = *((uint32_t*)(mbr + entry_offset + 12));

        if (entry.sector_count == 0 || entry.start_sector == 0)
            continue;

        kprintf("    part #%d: fs_type=%s, start_sector=%ld\n", i,
                (entry.fs_id == 0x83) ? "linux" : "unknown",
                entry.start_sector);

        // probe_fs();

        bd_add_part(bd, entry);
    }
}
