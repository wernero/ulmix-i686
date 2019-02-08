#include "ethernet.h"
#include <util/util.h>
#include <errno.h>

#define ETH_MAX_PAYLOAD 1500
#define ETH_MIN_LENGTH  60

#include <kdebug.h>

ssize_t ethernet_send(struct netdev_struct *netdev, mac_t dest, uint16_t proto, unsigned char *payload, size_t count)
{
    if (netdev == NULL)
        return -ECOMM;

    if (count > ETH_MAX_PAYLOAD)
        return -EMSGSIZE;

    int padding = ETH_MIN_LENGTH - (count + sizeof(struct eth_header));
    if (padding < 0)
        padding = 0;

    klog(KLOG_INFO, "ethernet_send: size=%d, padding=%d, total=%d", count, padding, count + sizeof(struct eth_header) + padding);

    unsigned char buffer[sizeof(struct eth_header) + count + padding];
    memset(buffer + sizeof(struct eth_header) + count, 0, padding);
    memcpy(buffer + sizeof(struct eth_header), payload, count);
    struct eth_header *header = (struct eth_header*)buffer;
    header->target_addr = dest;
    header->sender_addr = netdev->hw_addr;
    header->proto_type = flip_endian16(proto);

    return netdev->send(buffer, sizeof(struct eth_header) + count + padding);
}

void ethernet_recv(struct netdev_struct *netdev, unsigned char *packet, size_t size)
{
    klog(KLOG_INFO, "%s: size=%d, dest=%M, src=%M",
         netdev->name,
         size,
         (unsigned long)(packet),
         (unsigned long)(packet + 6)
         );
}
