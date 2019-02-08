#ifndef IP4_H
#define IP4_H

#include <util/types.h>
#include <inet/ethernet.h>

static mac_t ETH_BROADCAST =
{
    .addr[0] = 0xff,
    .addr[1] = 0xff,
    .addr[2] = 0xff,
    .addr[3] = 0xff,
    .addr[4] = 0xff,
    .addr[5] = 0xff
};

static mac_t ETH_EMPTY =
{
    .addr[0] = 0x00,
    .addr[1] = 0x00,
    .addr[2] = 0x00,
    .addr[3] = 0x00,
    .addr[4] = 0x00,
    .addr[5] = 0x00
};

ipaddr_t IPADDR(uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3);

void arp(ipaddr_t target);

#endif // IP4_H
