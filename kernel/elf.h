#ifndef ELF_H
#define ELF_H

#include <util/types.h>

struct elf_header_struct
{
    char		magic[4];
    uint8_t		mode;		// 1 = 32 bit, 2 = 64bit
    uint8_t		endianness;	// 1 = little, 2 = big
    uint8_t		version;
    uint8_t		abi;
    char		pad0[8];	// unused
    uint16_t	type;		// 1 = reloc, 2 = exec,
                            // 3 = shared, 4 = core
    uint16_t	ins_set;	// 3 = x86
    char		version2[4];
    uint32_t	entry_point;
    uint32_t	phtable_pos;
    uint32_t	shtable_pos;
    uint32_t	flags;		// arch dependent
    uint16_t	header_size;

    uint16_t	pht_size;
    uint16_t	pht_entries;
    uint16_t	sht_size;
    uint16_t	sht_entries;
    uint16_t	sht_index;
};

struct elf_pht_entry_struct
{
    uint32_t type;      // 0 = ignore, 1 = set to zero and then load,
                        // 2 = dynamic linking, 3 = interpreter
    uint32_t p_file;    // file offset of data
    uint32_t p_vaddr;   // location in virtual memory
    char     pad0[4];
    uint32_t p_filesz;  // size in file
    uint32_t p_memsz;   // size in memory
    uint32_t flags;     // exec, read, write
    uint32_t alignment; // required alignment
};

int elf_read_header(int fd, struct elf_header_struct **header);
int elf_get_pht_entry(int fd, int index, struct elf_header_struct *header, struct elf_pht_entry_struct *entry);

#endif // ELF_H
