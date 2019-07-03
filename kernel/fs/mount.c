#include <fs.h>
#include <devices.h>
#include <errno.h>
#include <vfs.h>

int do_mount(int dev_major, int dev_minor, const char *mount_path)
{
    struct hd_struct *part = get_hdstruct(dev_major, dev_minor);
    if (part == NULL)
        return -ENODEV;

    const struct fs_struct *fs = probe_fs(part);
    if (fs == NULL)
        return -EFSNOTSUP;

    // TODO: resolve pathname
    struct dir_struct *mnt_point = NULL;

    return fs->fops.fs_mount(part, mnt_point);
}
