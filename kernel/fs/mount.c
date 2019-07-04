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

    int error;
    struct direntry_struct *de;
    if ((error = namei(mount_path, &de)) < 0)
        return error;

    return fs->fops.fs_mount(part, de->directory);
}
