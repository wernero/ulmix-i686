/**
 * 
 * block io interface 
 * 
 * 
 */

#include "filesystem/ext2.h"
#include "errno.h"
#include "memory/kheap.h"
#include "log.h"
#include "drivers/devices.h"
#include "drivers/blockio.h"
#include "filesystem/filesystem.h"
#include "filesystem/fs_syscalls.h"
#include "filesystem/vfscore.h"




// block io prep 
// here should the request buffer also go ... 



int blockio_read(struct gendisk_struct *bd, char *buf, size_t block, size_t len) {

	  bd->fops.seek(bd->drv_struct, block, SEEK_SET);
	  bd->fops.read(bd->drv_struct, buf, len);
	  		  
//    return -EIO;
    return len;
}

int blockio_write(struct gendisk_struct *bd, char *buf, size_t block, size_t len) {

	  bd->fops.seek(bd->drv_struct, block, SEEK_SET);
	  bd->fops.read(bd->drv_struct, buf, len);
	  		  
//    return -EIO;
    return len;
}

