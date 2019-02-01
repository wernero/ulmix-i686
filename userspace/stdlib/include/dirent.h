#ifndef DIRENT_H
#define DIRENT_H

typedef struct _dirstream_struct
{
    int fileno;
} DIR;

struct dirent
{
   unsigned long  d_ino;       /* Inode number */
   unsigned long  d_off;       /* Not an offset; see below */
   unsigned short d_reclen;    /* Length of this record */
   unsigned char  d_type;      /* Type of file; not supported
                                  by all filesystem types */
   char           d_name[256]; /* Null-terminated filename */
};

DIR *opendir(char *name);
int closedir(DIR *dirp);
struct dirent* readdir(DIR *dirp);



#endif // DIRENT_H
