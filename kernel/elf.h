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


#endif // ELF_H
