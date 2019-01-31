#ifndef UNAME_H
#define UNAME_H

struct utsname
{
    char sysname[33];
    char nodename[33];
    char release[33];
    char version[33];
    char machine[33];
    char buildtime[33];
};

int sys_uname(struct utsname *buf);

#endif // UNAME_H
