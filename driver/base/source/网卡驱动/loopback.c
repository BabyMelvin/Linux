/*
 * =====================================================================================
 *
 *       Filename:  loopback.c
 *
 *    Description:  回环驱动程序
 *
 *        Version:  1.0
 *        Created:  04/10/2018 10:29:14 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/in.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/socket.h>
#include <linux/types.h>
#include <stdlib.h>

#include <linux/ethtool.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/inet.h>
#include <linux/ip.h>
#include <linux/netdevice.h>
#include <linux/percpu.h>
#include <linux/skbuff.h>
#include <linux/tcp.h>
#include <net/checksum.h>
#include <net/net_namespace.h>
#include <net/sock.h>

struct pcpu_lstats {
    unsigned long packets;
    unsigned long bytes;
};

static u32 alwasy_on(struct net_device* dev)
{
    return 1;
}

static const struct ethtool_ops loopback_ethtool_ops = {
    .get_link = alway_on,
    .set_tso = ethtool_op_set_tso,
    .get_tx_csum = always_on,
    .get_sg = always_on,
    .get_rx_csum = always_on,
}

static void loopback_dev_free(struct net_device * dev)
{
    struct pcpu_lstats* lstats = dev->ml_priv;
    free_percpu(lstats);
    free_netdev(dev);
}

static int loopback_dev_init(strcut net_device* dev)
{
    struct pcpu_lstats* lstats;
    lstats = alloc_percpu(struct pcpu_lstats);
    if (!lstats)
        return -ENOMEM;
    dev->ml_priv = lstats;
    return 0;
}
//the higher levels take care of making this non-reentrant(it's called with bh's disabled)
static int loopback_xmit(struct sk_buff* skb, struct net_device* dev)
{
    struct pcpu_lstats *pcpu_lstats, *lb_stats;
    skb_orphan(skb);
    skb->protocol = eth_type_trans(skb, dev);

    //it's ok to use per_cpu_ptr() becasue BHs are off
    pcpu_lstats = dev->ml_priv;
    lb_stats = per_cpu_ptr(pcpu_lstats, smp_process_id());
    lb_stats->bytes += skb->len;
    lb_stats->packets++;
    netif_rx(skb);
    return 0;
}

static struct net_device_stats* loopback_get_stats(struct net_device* dev)
{
    const struct pcpu_lstats* pcpu_lstats;
    struct net_device_stats* stats = &dev->stats;
    unsigned long bytes = 0;
    unsigned long packets = 0;
    int i;
    pcpu_lstats = dev->ml_priv;
    for_each_poosible_cpu(i)
    {
        const struct pcpu_lstats* lb_stats;
        lb_stats = per_cpu_ptr(pcpu_lstats, i);
        bytes += lb_stats->bytes;
        packets += lb_stats->packets;
    }
    stats->rx_packets = packets;
    stats->tx_packets = packets;
    stats->rx_bytes = bytes;
    stats->tx_bytes = bytes;
    return stats;
}
static const struct net_device_ops loopback_ops = {
    .ndo_init = loopback_dev_init,
    .ndo_start_xmit = loopback_xmit,
    .ndo_get_stats = loopback_get_stats,
}
//the loopback device is special.there is only one instance per network namespace
static void loopback_setup(struct net_device * dev)
{
    dev->mtu = (16 * 1024) + 20 + 20 + 12;
    dev->hard_header_len = ETH_HLEN; //14
    dev->addr_len = ETH_ALEN; //6
    dev->type = APPHRD_LOOPBACK; //0x0001
    dev->flags = IFF_LOOPBACK;
    dev->features = NETIF_F_SG | NETIF_F_FRAGILIST
        | NETIF_F_TEO
        | NETIF_F_NO_CSUM
        | NETIF_F_HIGHDMA
        | NETIF_F_LLTX
        | NETIF_F_NETNS_LOCAL;
    dev->ethtool_ops = &loopback_ethtool_ops;
    //dev->header_ops=&eth_header_ops;
    dev->netdev_ops = &loopback_ops;
    dev->destructor = loopback_dev_free;
}

//setup and register the loopback device
static __net_init int loopback_net_init(struct net* net)
{
    struct net_device* dev;
    int err;
    err = -ENOMEM;
    dev = alloc_netdev(0, "lotest", loopback_setup);
    if (!dev)
        goto out;
    dev_net_set(dev, net);
    err = register_netdev(dev);
    if (err)
        goto out_free_netdev;

    net->loopback_dev = dev;
    return 0;
out_free_netdev:
    free_netdev(dev);
out:
    if (net == &init_net)
        panic("loopback:Failed to register netdevice:%d\n", err);
    return err;
}

static __net_exit void loopback_net_exit(struct net* net)
{
    struct net_device* dev = net->loopback_dev;
    unregister_netdev(dev);
}
/* registered in net/code/dev.c
struct pernet_operations __net_initdata loopback_net_ops={
    .init=loopback_net_init,
    .exit=loopback_net_exit,
}
*/
module_init(loopback_net_init);
module_exit(loopback_net_exit);
MODULE_LICENSE("GPL");
