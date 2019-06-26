#include <devices.h>

#define MAX_DEVICES 32

static struct chardev_struct *cd_list[MAX_DEVICES];
static struct gendisk_struct *bd_list[MAX_DEVICES];

int register_chardev(struct chardev_struct *cd)
{
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (cd_list[i] == NULL)
        {
            cd_list[i] = cd;
            return 0;
        }
    }

    return -1;
}

int register_blkdev(struct gendisk_struct *bd)
{
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (bd_list[i] == NULL)
        {
            bd_list[i] = bd;
            return 0;
        }
    }

    return -1;
}

struct chardev_struct *get_chardev(unsigned int major, unsigned int minor)
{
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (cd_list[i] != NULL)
        {
            if (cd_list[i]->major == major
                    && cd_list[i]->minor == minor)
                return cd_list[i];
        }
    }

    return NULL;
}

struct gendisk_struct *get_blkdev(unsigned int major, unsigned int minor)
{
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (bd_list[i] != NULL)
        {
            if (bd_list[i]->major == major
                    && bd_list[i]->minor == minor)
                return bd_list[i];
        }
    }

    return NULL;
}
