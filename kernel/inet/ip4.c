#include "ip4.h"

extern struct netdev_struct *current_netdev;

#define ARP_HW_ETHERNET 0x0001
#define ARP_PROTO_IP4   0x0800
#define ARP_OP_REQUEST  0x0001

struct arp_request
{
    uint16_t        hw_type;
    uint16_t        proto_type;
    uint8_t         hw_size;
    uint8_t         proto_size;
    uint16_t        opcode;
    mac_t           sender_mac;
    ipaddr_t        sender_ip;
    mac_t           target_mac;
    ipaddr_t        target_ip;
} __attribute__((packed));

ipaddr_t IPADDR(uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3)
{
    ipaddr_t ip;
    ip = ip3 | (ip2 << 8) | (ip1 << 16) | (ip0 << 24);
    return ip;
}

void arp(ipaddr_t target)
{
    struct arp_request arp_rq = {
        .hw_type    = flip_endian16(ARP_HW_ETHERNET),
        .proto_type = flip_endian16(ARP_PROTO_IP4),
        .hw_size    = 0x06,
        .proto_size = 0x04,
        .opcode     = flip_endian16(ARP_OP_REQUEST),
        .sender_mac = current_netdev->hw_addr,
        .sender_ip  = current_netdev->ip_addr,
        .target_mac = ETH_EMPTY,
        .target_ip  = target
    };
    ethernet_send(current_netdev, ETH_BROADCAST, ETH_PROTO_ARP, (unsigned char *)&arp_rq, sizeof(struct arp_request));
}
