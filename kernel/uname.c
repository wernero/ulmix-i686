#include <uname.h>
#include <kconfig.h>
#include <util/util.h>

int sys_uname(struct utsname *buf)
{
    USERPTR(buf);

    strcpy(buf->version, VERSION);
    strcpy(buf->machine, ARCH);
    strcpy(buf->release, RELEASE);
    strcpy(buf->sysname, SYSNAME);
    strcpy(buf->buildtime, __DATE__ " " __TIME__);

    *buf->nodename = 0; // not significant
    return SUCCESS;
}
