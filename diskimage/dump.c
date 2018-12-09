
#include <stdio.h>
//#include <stdlib.h>
#include <linux/types.h>


#define IMG_START_EXT2			512
#define IMG_START_SB			(IMG_START_EXT2 + 1024)

#define EXT2_BLOCK_SIZE			0x400

/*
 * Constants relative to the data blocks
 */
#define	EXT2_NDIR_BLOCKS		12
#define	EXT2_IND_BLOCK			EXT2_NDIR_BLOCKS
#define	EXT2_DIND_BLOCK			(EXT2_IND_BLOCK + 1)
#define	EXT2_TIND_BLOCK			(EXT2_DIND_BLOCK + 1)
#define	EXT2_N_BLOCKS			(EXT2_TIND_BLOCK + 1)

#define EXT2_NAME_LEN 255


// from uapi/linux/types.h
typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;
typedef __u64 __bitwise __le64;
typedef __u64 __bitwise __be64;

typedef __u16 __bitwise __sum16;
typedef __u32 __bitwise __wsum;


/*
 * Structure of a blocks group descriptor
 */
struct ext2_group_desc
{
	__le32	bg_block_bitmap;		/* Blocks bitmap block */
	__le32	bg_inode_bitmap;		/* Inodes bitmap block */
	__le32	bg_inode_table;		/* Inodes table block */
	__le16	bg_free_blocks_count;	/* Free blocks count */
	__le16	bg_free_inodes_count;	/* Free inodes count */
	__le16	bg_used_dirs_count;	/* Directories count */
	__le16	bg_pad;
	__le32	bg_reserved[3];
};


/*
 * Structure of an inode on the disk
 */
struct ext2_inode {
	__le16	i_mode;		/* File mode */
	__le16	i_uid;		/* Low 16 bits of Owner Uid */
	__le32	i_size;		/* Size in bytes */
	__le32	i_atime;	/* Access time */
	__le32	i_ctime;	/* Creation time */
	__le32	i_mtime;	/* Modification time */
	__le32	i_dtime;	/* Deletion Time */
	__le16	i_gid;		/* Low 16 bits of Group Id */
	__le16	i_links_count;	/* Links count */
	__le32	i_blocks;	/* Blocks count */
	__le32	i_flags;	/* File flags */
	union {
		struct {
			__le32  l_i_reserved1;
		} linux1;
		struct {
			__le32  h_i_translator;
		} hurd1;
		struct {
			__le32  m_i_reserved1;
		} masix1;
	} osd1;				/* OS dependent 1 */
	__le32	i_block[EXT2_N_BLOCKS];/* Pointers to blocks */
	__le32	i_generation;	/* File version (for NFS) */
	__le32	i_file_acl;	/* File ACL */
	__le32	i_dir_acl;	/* Directory ACL */
	__le32	i_faddr;	/* Fragment address */
	union {
		struct {
			__u8	l_i_frag;	/* Fragment number */
			__u8	l_i_fsize;	/* Fragment size */
			__u16	i_pad1;
			__le16	l_i_uid_high;	/* these 2 fields    */
			__le16	l_i_gid_high;	/* were reserved2[0] */
			__u32	l_i_reserved2;
		} linux2;
		struct {
			__u8	h_i_frag;	/* Fragment number */
			__u8	h_i_fsize;	/* Fragment size */
			__le16	h_i_mode_high;
			__le16	h_i_uid_high;
			__le16	h_i_gid_high;
			__le32	h_i_author;
		} hurd2;
		struct {
			__u8	m_i_frag;	/* Fragment number */
			__u8	m_i_fsize;	/* Fragment size */
			__u16	m_pad1;
			__u32	m_i_reserved2[2];
		} masix2;
	} osd2;				/* OS dependent 2 */
};





/*
 * Structure of the super block
 */
struct ext2_super_block {
	__le32	s_inodes_count;		/* Inodes count */
	__le32	s_blocks_count;		/* Blocks count */
	__le32	s_r_blocks_count;	/* Reserved blocks count */
	__le32	s_free_blocks_count;	/* Free blocks count */
	__le32	s_free_inodes_count;	/* Free inodes count */
	__le32	s_first_data_block;	/* First Data Block */
	__le32	s_log_block_size;	/* Block size */
	__le32	s_log_frag_size;	/* Fragment size */
	__le32	s_blocks_per_group;	/* # Blocks per group */
	__le32	s_frags_per_group;	/* # Fragments per group */
	__le32	s_inodes_per_group;	/* # Inodes per group */
	__le32	s_mtime;		/* Mount time */
	__le32	s_wtime;		/* Write time */
	__le16	s_mnt_count;		/* Mount count */
	__le16	s_max_mnt_count;	/* Maximal mount count */
	__le16	s_magic;		/* Magic signature */
	__le16	s_state;		/* File system state */
	__le16	s_errors;		/* Behaviour when detecting errors */
	__le16	s_minor_rev_level; 	/* minor revision level */
	__le32	s_lastcheck;		/* time of last check */
	__le32	s_checkinterval;	/* max. time between checks */
	__le32	s_creator_os;		/* OS */
	__le32	s_rev_level;		/* Revision level */
	__le16	s_def_resuid;		/* Default uid for reserved blocks */
	__le16	s_def_resgid;		/* Default gid for reserved blocks */
	/*
	 * These fields are for EXT2_DYNAMIC_REV superblocks only.
	 *
	 * Note: the difference between the compatible feature set and
	 * the incompatible feature set is that if there is a bit set
	 * in the incompatible feature set that the kernel doesn't
	 * know about, it should refuse to mount the filesystem.
	 * 
	 * e2fsck's requirements are more strict; if it doesn't know
	 * about a feature in either the compatible or incompatible
	 * feature set, it must abort and not try to meddle with
	 * things it doesn't understand...
	 */
	__le32	s_first_ino; 		/* First non-reserved inode */
	__le16   s_inode_size; 		/* size of inode structure */
	__le16	s_block_group_nr; 	/* block group # of this superblock */
	__le32	s_feature_compat; 	/* compatible feature set */
	__le32	s_feature_incompat; 	/* incompatible feature set */
	__le32	s_feature_ro_compat; 	/* readonly-compatible feature set */
	__u8	s_uuid[16];		/* 128-bit uuid for volume */
	char	s_volume_name[16]; 	/* volume name */
	char	s_last_mounted[64]; 	/* directory where last mounted */
	__le32	s_algorithm_usage_bitmap; /* For compression */
	/*
	 * Performance hints.  Directory preallocation should only
	 * happen if the EXT2_COMPAT_PREALLOC flag is on.
	 */
	__u8	s_prealloc_blocks;	/* Nr of blocks to try to preallocate*/
	__u8	s_prealloc_dir_blocks;	/* Nr to preallocate for dirs */
	__u16	s_padding1;
	/*
	 * Journaling support valid if EXT3_FEATURE_COMPAT_HAS_JOURNAL set.
	 */
	__u8	s_journal_uuid[16];	/* uuid of journal superblock */
	__u32	s_journal_inum;		/* inode number of journal file */
	__u32	s_journal_dev;		/* device number of journal file */
	__u32	s_last_orphan;		/* start of list of inodes to delete */
	__u32	s_hash_seed[4];		/* HTREE hash seed */
	__u8	s_def_hash_version;	/* Default hash version to use */
	__u8	s_reserved_char_pad;
	__u16	s_reserved_word_pad;
	__le32	s_default_mount_opts;
 	__le32	s_first_meta_bg; 	/* First metablock block group */
	__u32	s_reserved[190];	/* Padding to the end of the block */
};





/*
 * The new version of the directory entry.  Since EXT2 structures are
 * stored in intel byte order, and the name_len field could never be
 * bigger than 255 chars, it's safe to reclaim the extra byte for the
 * file_type field.
 */
struct ext2_dir_entry_2 {
	__le32	inode;			/* Inode number */
	__le16	rec_len;		/* Directory entry length */
	__u8	name_len;		/* Name length */
	__u8	file_type;
	char	name[];			/* File name, up to EXT2_NAME_LEN */
};


long block_to_address(__u32 block) {

	if(block == 0) return 0;

	return ((block * EXT2_BLOCK_SIZE) + IMG_START_EXT2 );
}


int main() {

	int n;
	int g,i,j;
	struct ext2_super_block sb;
	struct ext2_group_desc gd[10];
	struct ext2_inode id;

	__u32 ext2_inode_indirect_ptr[256];

	int num_block_groups = 0;

	//directory data
	struct ext2_dir_entry_2 de;



	FILE *fptr;

	if ((fptr = fopen("hdd.img","rb")) == NULL){
		printf("Error! opening file");

		// Program exits if the file pointer returns NULL.
		return 1;
	}

   	// seek to start of superblock => 512 MBR + 1024 ext2 bootblock => 0x400;
   	// as image file is a disk image file
   	fseek(fptr,IMG_START_SB,SEEK_SET);  // SEEK_SET is from start; SEEK_CUR from current position

   	// read superblock from file
   	fread(&sb, sizeof(struct ext2_super_block), 1, fptr); 
   	printf("s_magic: %x\n", sb.s_magic);
   	printf("s_inode_count: %d\n", sb.s_inodes_count);
   	printf("s_blocks_count: %d\n", sb.s_blocks_count);
	printf("s_free_blocks_count: %d\n", sb.s_free_blocks_count);	/* Free blocks count */
	printf("s_free_inodes_count: %d\n", sb.s_free_inodes_count);	/* Free inodes count */
	printf("s_first_data_block: %d\n", sb.s_first_data_block);	/* First Data Block */
	printf("s_log_block_size: %d\n", sb.s_log_block_size);	/* Block size */
	printf("s_log_frag_size: %d\n", sb.s_log_frag_size);	/* Fragment size */
	printf("s_blocks_per_group: %d\n", sb.s_blocks_per_group);	/* # Blocks per group */
	printf("s_frags_per_group: %d\n", sb.s_frags_per_group);	/* # Fragments per group */
	printf("s_inodes_per_group: %d\n", sb.s_inodes_per_group);	/* # Inodes per group */

   	num_block_groups = sb.s_blocks_count / sb.s_blocks_per_group + 1; // check roundup 

   	printf("size group struct %x\n", sizeof(struct ext2_group_desc));

   	// read group blocks
   	for(g=0; g < num_block_groups; g++)
   	{
	   	fread(&(gd[g]), sizeof(struct ext2_group_desc), 1, fptr);
	   	printf("%d: bg_block_bitmap: %x - %x \n",g , gd[g].bg_block_bitmap, block_to_address(gd[g].bg_block_bitmap));
	   	printf("%d: bg_inode_bitmap: %x - %x\n",g , gd[g].bg_inode_bitmap, block_to_address(gd[g].bg_inode_bitmap));
	   	printf("%d: bg_inode_table: %x - %x \n",g , gd[g].bg_inode_table, block_to_address(gd[g].bg_inode_table));
	   	printf("%d: bg_free_blocks_count: %d\n",g , gd[g].bg_free_blocks_count);
		printf("%d: bg_free_inodes_count: %d\n",g , gd[g].bg_free_inodes_count);
		printf("%d: bg_used_dirs_count: %d\n",g , gd[g].bg_used_dirs_count);

   	
	}


   	for(g=0; g < num_block_groups; g++)
   	{

   		long fptr_pos = 0;

	   	// jump to Inode data
	   	fseek(fptr,block_to_address(gd[g].bg_inode_table),SEEK_SET);

	   	printf("reading inode data for block group %d\n", g);

	   	// read inode data for group
	   	for(i=0; i < sb.s_inodes_per_group; i++)
	   	{

//	   		printf("pos in image: %x\n", ftell(fptr));
		   	fread(&id, sizeof(struct ext2_inode), 1, fptr);
		   	

		   	if(id.i_mode > 0) {
			   	printf("%d - %d: i_mode: %4x\n",g, i+1 , id.i_mode);
			   	if(id.i_mode & 0x4000) {
			   		printf("entry is a direcotry\n");
			   	}
			   	if(id.i_mode & 0x8000) {
			   		printf("entry is a file\n");
			   	}
			   	printf("%d - %d: i_uid: %x\n",g ,i+1 , id.i_uid);
			   	printf("%d - %d: i_size: %d\n",g ,i+1 , id.i_size);
				printf("%d - %d: i_blocks (512b): %d\n",g ,i+1 , id.i_blocks);   // block size is always assumed in 512bye
				printf("%d - %d: i_flags: %d\n",g ,i+1 , id.i_flags);


				for(n=0; n < EXT2_N_BLOCKS; n++) {

					printf("%d - %d: i_block[ %d ]: %x\n",g ,i+1, n , block_to_address(id.i_block[n]));


			   		if(block_to_address(id.i_block[n])) {
		   			// address not zero; parse content at location


					   	if(id.i_mode & 0x4000) {   // inode is a directory entry
					   		fptr_pos = ftell(fptr);


	// __le32	inode;			/* Inode number */
	// __le16	rec_len;		/* Directory entry length */
	// __u8	name_len;		/* Name length */
	// __u8	file_type;
	// char	name[];			/* File name, up to EXT2_NAME_LEN */
 
			   				fseek(fptr,block_to_address(id.i_block[n]), SEEK_SET);

				   			while (1) {
					   			fread(&de.inode, sizeof(__le32), 1, fptr);
					   			if(de.inode == 0) break;
					   			fread(&de.rec_len, sizeof(__le16), 1, fptr);
					   			fread(&de.name_len, sizeof(__u8), 1, fptr);
					   			fread(&de.file_type, sizeof(__u8), 1, fptr);

					   			// to read name - the name_len has to be checked as it is aligned to 4
					   			// this methode does not cover delete directory entries; which are still in the list;
					   			// but would normaly be skipped as the rec_len field would skip over it
					   			if((de.name_len % 4) > 0) { // number is not multple of 4
					   				de.name_len = ((de.name_len / 4) + 1) * 4;

					   			}
					   			fread(&de.name, sizeof(char), de.name_len, fptr);

					   			printf("%d - %d: de_inode: %d\n",g ,i+1 , de.inode);
					   			printf("%d - %d: de_rec_len: %d\n",g ,i+1 , de.rec_len);
					   			printf("%d - %d: de_name_len: %d\n",g ,i+1 , de.name_len);
					   			printf("%d - %d: de_name: >> %s <<\n",g ,i+1 , de.name);

					   			if(de.rec_len > (EXT2_NAME_LEN + 8)) break; // end of directory records
					   		}
					   		fseek(fptr,fptr_pos, SEEK_SET);

				   		} // end directory


					   	if(id.i_mode & 0x8000) {   // inode is a directory entry

					   		switch(n) {
					   			case 12: // single indirect
					   				printf("inode data single indirect\n");
							   		fptr_pos = ftell(fptr); // save current position


					   				fseek(fptr,block_to_address(id.i_block[n]), SEEK_SET);
						   			fread(&ext2_inode_indirect_ptr, sizeof(ext2_inode_indirect_ptr), 1, fptr);

						   			for(j = 0; j < 256; j++) {

						   				printf("%d - %d: i_block[ %d ] single indrect [ %d ]: %x\n",g ,i+1, n, j, block_to_address(ext2_inode_indirect_ptr[j]));

						   			}

							   		fseek(fptr,fptr_pos, SEEK_SET); // return to current position
					   				break;
					   			case 13: // double indirect
					   				printf("inode data double indirect\n");
							   		fptr_pos = ftell(fptr); // save current position


					   				fseek(fptr,block_to_address(id.i_block[n]), SEEK_SET);
						   			fread(&ext2_inode_indirect_ptr, sizeof(ext2_inode_indirect_ptr), 1, fptr);

						   			for(j = 0; j < 256; j++) {

						   				printf("%d - %d: i_block[ %d ] double indrect [ %d ]: %x\n",g ,i+1, n, j, block_to_address(ext2_inode_indirect_ptr[j]));

						   			}

							   		fseek(fptr,fptr_pos, SEEK_SET); // return to current position


					   				break;
					   			case 14: // triple indirect
					   				printf("inode data triple indirect\n");
					   				break;

					   			default:
					   				break;
					   		}

					   	} // end file


				   		
				   	} // address not zero

					
				}

		   	}

	   	}

    }

   	fclose(fptr);

   	return 0;

}
