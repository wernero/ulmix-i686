#include "netdev.h"
#include <inet/ip4.h>
#include <memory/kheap.h>

struct netdev_struct *current_netdev;

void setup_inet()
{

}

struct netdev_struct *netdev_register(struct netdev_struct netdev)
{
    current_netdev = kmalloc(sizeof(struct netdev_struct), 1, "netdev_struct");
    *current_netdev = netdev;
    return current_netdev;
}
