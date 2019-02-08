#ifndef NETDEV_H
#define NETDEV_H

#include <util/types.h>

static inline uint16_t flip_endian16(uint16_t nb)
{
    return (nb >> 8) | (nb << 8);
}

struct netdev_struct
{
    char *name;
    mac_t hw_addr;
    ipaddr_t ip_addr;

    ssize_t (*send)(unsigned char *mesg, size_t count);
};

void setup_inet(void);
void netdev_register(struct netdev_struct netdev);

#endif // NETDEV_H
