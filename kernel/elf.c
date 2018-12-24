#include "elf.h"
#include <memory/kheap.h>
#include <errno.h>
#include <filesystem/fs_syscalls.h>

int elf_read_header(int fd, struct elf_header_struct **header)
{
    *header = kmalloc(sizeof(struct elf_header_struct), 1, "elf header");
    if (sc_read(fd, *header, sizeof(struct elf_header_struct)) < 0)
    {
        return -EIO;
    }

    char *magic = (*header)->magic;
    if (magic[0] != 0x7f || magic[1] != 'E' ||
            magic[2] != 'L' || magic[3] != 'F')
        return -ENOEXEC;

    if ((*header)->mode != 1               // not 32bit
         || (*header)->endianness != 1     // not little endian
         || (*header)->type != 2           // not an executable
         || (*header)->ins_set != 3)       // not x86
        return -ENOEXEC;

    return 0;
}

int elf_get_pht_entry(int fd, int index, struct elf_header_struct *header, struct elf_pht_entry_struct *entry)
{
    sc_lseek(fd, header->phtable_pos + index * sizeof(struct elf_pht_entry_struct), SEEK_SET);
    if (sc_read(fd, entry, sizeof(struct elf_pht_entry_struct)) < 0)
    {
        return -EIO;
    }
    return 0;
}
