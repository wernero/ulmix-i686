#ifndef ETHERNET_H
#define ETHERNET_H

#include <inet/netdev.h>
#include <util/types.h>

#define ETH_PROTO_ARP   0x0806

struct eth_header
{
    mac_t       target_addr;
    mac_t       sender_addr;
    uint16_t    proto_type;
} __attribute((packed));

ssize_t ethernet_send(struct netdev_struct *netdev, mac_t dest, uint16_t proto, unsigned char *payload, size_t count);
void ethernet_recv(struct netdev_struct *netdev, unsigned char *packet, size_t size);

#endif // ETHERNET_H
