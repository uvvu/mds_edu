/***************************************
 * Filename: snull.c
 * Title: Virtual Network Device
 * Desc: network driver module
****************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h>

#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>

MODULE_LICENSE("GPL");

int snull_init(struct net_device *dev)
{
    printk("snull init...\n");
    ether_setup(dev);
    
    return 0;
}

//static struct net_device snull_dev;
struct net_device *dev = NULL;

char snull_name[16];

static int snull_init_module(void)
{
    //struct net_device *dev = &snull_dev;
    int result;

	dev = alloc_etherdev(sizeof(struct net_device));

    printk("snull up...\n");
    
    strcpy(dev->name, "sn0");
    result = register_netdev(dev);
    return 0;
}

static void snull_exit_module(void)
{
    printk("snull down...\n");
    unregister_netdev(dev);
}

module_init(snull_init_module);
module_exit(snull_exit_module);
