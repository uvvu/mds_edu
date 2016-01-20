#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO	*/
#include <linux/init.h>		/* Needed for the macros */


static int __init init_hello_4(void)
{
	printk(KERN_INFO "Hello, world 4.\n");
	return 0;
}

static void __exit cleanup_hello_4(void)
{
	printk(KERN_INFO "Goodbye, world 4.\n");
}

module_init(init_hello_4);
module_exit(cleanup_hello_4);

/* Get rid of taint message by declaring code as GPL. */
MODULE_LICENSE("GPL");
MODULE_SUPPORTED_DEVICE("testdevice");


