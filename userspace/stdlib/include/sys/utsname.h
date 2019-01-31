#ifndef UTSNAME_H
#define UTSNAME_H

struct utsname
{
    char sysname[33];
    char nodename[33];
    char release[33];
    char version[33];
    char machine[33];
    char buildtime[33];
};

int uname(struct utsname *buf);

#endif // UTSNAME_H
