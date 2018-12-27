/**
 * 
 * block io interface 
 * 
 * 
 */

#ifndef BLOCKIO_H
#define BLOCKIO_H


int blockio_read(struct gendisk_struct *bd, char *buf, size_t block, size_t len);

int blockio_write(struct gendisk_struct *bd, char *buf, size_t block, size_t len);


#endif // BLOCKIO_H