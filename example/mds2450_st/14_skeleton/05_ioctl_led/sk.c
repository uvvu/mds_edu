/***************************************
 * Filename: sk.c
 * Title: Skeleton Device
 * Desc: Implementation of system call
 ***************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/major.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/regs-gpio.h> //SFR

#include "sk.h"

MODULE_LICENSE("GPL");

static int sk_major = 0, sk_minor = 0;
static int result;
static dev_t sk_dev;

static struct cdev sk_cdev;

static int sk_register_cdev(void);

static int sk_open(struct inode *inode, struct file *filp);
static int sk_release(struct inode *inode, struct file *filp);
static int sk_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static int sk_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static long sk_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

static int sk_open(struct inode *inode, struct file *filp)
{
    printk("Device has been opened...\n");
    
    /* H/W Initalization */
    
    return 0;
}

static int sk_release(struct inode *inode, struct file *filp)
{
    printk("Device has been closed...\n");
    
    return 0;
}

static int sk_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	char data[11];
	int ret;

	ret = copy_from_user(data, buf, count);
	if (ret<0) return -1;
	printk("data >>>>> = %s\n", data);

	return count;
}

static int sk_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	char data[20] = "this is read func...";
	int ret;
	ret = copy_to_user(buf, data, count);
	if(ret<0) return -1;

	return 0;
}

static long sk_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	// sk_info ctrl_info;
	int size, err;

	if(_IOC_TYPE(cmd) != SK_MAGIC) {
		printk(" _IOC_TYPE error\n");
		return -EINVAL;
	};
	if(_IOC_NR(cmd) >= SK_MAXNR) {
		printk("_IOC_NR error\n");
		return -EINVAL;
	};
	size = _IOC_SIZE(cmd);

	// POINT: check commands
	if (size) {
		err = 0;
		if(_IOC_DIR(cmd) & _IOC_READ)
			err = access_ok(VERIFY_READ, (void *)arg, size);
		else if (_IOC_DIR(cmd) & _IOC_WRITE)
			err = access_ok(VERIFY_WRITE, (void *)arg, size);
		if (err) return err;
	}

	// TODO: allows to access the LED device using ioctl commands
	switch(cmd) {
		case SK_LED_OFF: {
			printk("cmd = 0\n");
			// TODO here
			gpio_set_value(S3C2410_GPG(4), 1);
			gpio_set_value(S3C2410_GPG(5), 1);
			gpio_set_value(S3C2410_GPG(6), 1);
			gpio_set_value(S3C2410_GPG(7), 1);
			break;	
		}
		case SK_LED_ON: {
			printk("cmd = 1\n");
			gpio_set_value(S3C2410_GPG(4), 0);
			gpio_set_value(S3C2410_GPG(5), 0);
			gpio_set_value(S3C2410_GPG(6), 0);
			gpio_set_value(S3C2410_GPG(7), 0);
			break;
		}
		case SK_GETSTATE: {
			printk("cmd = 2\n"); 
			break;
		}
		default:
			return 0;
	}
	return 0;
}

struct file_operations sk_fops = { 
    .open       = sk_open,
    .release    = sk_release,
	.write		= sk_write,
	.read		= sk_read,
	.unlocked_ioctl = sk_ioctl,
};

static int __init sk_init(void)
{
    int i;
	printk("SK Module is up... \n");

	if((result = sk_register_cdev()) < 0)
	{
		return result;
	}

	// TODO: initialize GPIO ports to use LED device
	// 1. off the leds
	// 2. set the leds to output mode
	for (i=4; i<=7; i++){
			gpio_request(S3C2410_GPG(i), "led");
			gpio_set_value(S3C2410_GPG(i), 1); //set GPGDAT
			s3c_gpio_cfgpin(S3C2410_GPG(i), S3C2410_GPIO_OUTPUT);
		}

		//gpio_direction_output(S3C2410_GPG(i), 1);
		//gpio_direction = gpio_request + gpio_set_value
		// --> set GPGCON, GPGDAT

    return 0;
}

static void __exit sk_exit(void)
{
	// TODO: exit GPIO Controls
	int i;
    printk("The module is down...\n");
	for (i=4; i<=7; i++){
		gpio_free(S3C2410_GPG(i));
	}	
	cdev_del(&sk_cdev);
	unregister_chrdev_region(sk_dev, 1);
}

static int sk_register_cdev(void)
{
	int error;

	/* allocation device number */
	if(sk_major) {
		sk_dev = MKDEV(sk_major, sk_minor);
		error = register_chrdev_region(sk_dev, 1, "sk");
	} else {
		error = alloc_chrdev_region(&sk_dev, sk_minor, 1, "sk");
		sk_major = MAJOR(sk_dev);
	}

	if(error < 0) {
		printk(KERN_WARNING "sk: can't get major %d\n", sk_major);
		return result;
	}
	printk("major number=%d\n", sk_major);

	/* register chrdev */
	cdev_init(&sk_cdev, &sk_fops);
	sk_cdev.owner = THIS_MODULE;
	sk_cdev.ops = &sk_fops;
	error = cdev_add(&sk_cdev, sk_dev, 1);

	if(error)
		printk(KERN_NOTICE "sk Register Error %d\n", error);

	return 0;
}

module_init(sk_init); 
module_exit(sk_exit);

