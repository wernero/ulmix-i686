#include <fs.h>
#include <errno.h>

#define MAX_FILESYSTEMS 16

static struct fs_struct const *filesystems[MAX_FILESYSTEMS];

int register_fs(const struct fs_struct *fs)
{
    for (int i = 0; i < MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] == NULL)
        {
            filesystems[i] = fs;
            return SUCCESS;
        }
    }

    return -EAGAIN;
}

const struct fs_struct *probe_fs(struct hd_struct *part)
{
    for (int i = 0; i < MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] != NULL)
        {
            if (filesystems[i]->mbr_id == part->fs_type)
            {
                if (filesystems[i]->fops.fs_probe(part) == SUCCESS)
                    return filesystems[i];
            }
        }
    }

    return NULL;
}
