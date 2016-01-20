#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init hello_init(void)
{
	printk(KERN_ALERT "Hello World!!\n");
	return 0;
}

static int __exit hello_exit(void)
{
	printk(KERN_ALERT "Good Bye Kernel~!!\n");
	return 0;
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TYLIM");
MODULE_DESCRIPTION("Hello Driver Module");



