/*
 * helloplat.c
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/pm.h>
#include <linux/platform_device.h>
#include <linux/earlysuspend.h>
#include <linux/device.h>
#include <linux/slab.h>

// module attributes
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Youngdeok");
MODULE_DESCRIPTION("Hello platform Module");

static char msg[100] = {0};
static short readPos = 0;
static int times = 0;

// prototypes 
static int dev_open(struct inode *, struct file *);
static int dev_close(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

// structure containing callbacks
static struct file_operations fops = 
{
	.read = dev_read,
	.open = dev_open,
	.write = dev_write,
	.release = dev_close,
};

struct miscdevice hello_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "hellomisc",
	.fops = &fops
};


// POINT: declare the android power management
struct hello_event {
	struct early_suspend early_suspend;
};


// called when open system call is done on the device file
static int dev_open(struct inode *inod, struct file *filp)
{
	times++;
	printk(KERN_ALERT "Misc Device opened %d times\n", times);
	return 0;
}

// called when read system call is done on the device file
static ssize_t dev_read(struct file *filp, char *buff, size_t len, loff_t *off)
{
	short count = 0;
	while (len && (msg[readPos] != 0 ))
	{
		// copy byte from kernel space to user space
		put_user(msg[readPos], buff++);
		count++;
		len--;
		readPos++;
	}
	return count;
}

// called when write system call is done on the device file
static ssize_t dev_write(struct file *filp, const char *buff, size_t len, loff_t *off)
{
	short ind = len - 1;
	short count = 0;
	memset(msg, 0, 100);
	readPos = 0;
	while ( len>0 ) {
		// copy the given string to the driver but in reverse
		msg[count++] = buff[ind--];
		len--;
	}
	return count;
}

// called when close system call is done on the device file
static int dev_close(struct inode *inod, struct file *filp)
{
	printk(KERN_ALERT "Device Closed\n");
	return 0;
}

void dev_early_suspend(struct early_suspend *h)
{
	printk("hello driver early_suspend\n");
}

void dev_late_resume(struct early_suspend *h)
{
	printk("hello driver late_resume\n");
}

// POINT: platform device probe function
// TODO: implement the dev_probe function
// 1) declare the hello_event pointer
// 2) register the hello_misc device
// 3) allocate hello_event pointer obeject using kzalloc()
// 4) initialize the member of early_suspend
static int dev_probe(struct platform_device *pdev)
{
	// TODO: code here
	
	return 0;
}

static int dev_remove(struct platform_device *pdev)
{
	struct hello_event *ev;
	printk("called hello driver remove!\n");
	unregister_early_suspend(&ev->early_suspend);
	return 0;
}

// TODO: define the hello_driver as platform_driver
// set the name to hellomisc
// called when modules is loaded
static int __init hello_init(void)
{
	printk("Hello Platform init - register platform driver\n");
	// TODO: register the hello_driver to platform_driver
	
}

// called when module in unloaded 
static void __exit hello_exit(void)
{
	return platform_driver_unregister(&hello_driver);
}

module_init(hello_init);
module_exit(hello_exit);

