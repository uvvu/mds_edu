#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO	*/
#include <linux/init.h>		/* Needed for the macros */

#define	DRIVER_AUTHOR	"Jong In, Char <jongin@mdstec.com>"
#define	DRIVER_DESC		"A sample driver"


static void __exit cleanup_hello_4(void)
{
	printk(KERN_INFO "Goodbye, world 4.\n");
}

module_exit(cleanup_hello_4);
/* Get rid of taint message by declaring code as GPL. */
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);		/* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC);	/* What does this module do */
MODULE_SUPPORTED_DEVICE("testdevice");