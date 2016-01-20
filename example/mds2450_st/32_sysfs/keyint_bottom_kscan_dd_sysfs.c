#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <plat/regs-gpio.h>
#include <plat/gpio-bank-h1.h>
#include <linux/workqueue.h>	//work queue

#include <linux/cdev.h>			//cdev_init
#include <linux/wait.h>			//wait_event_interruptible
#include <asm/uaccess.h>		//user access
#include <linux/fs.h>			//file_operatios 

#include <linux/platform_device.h> //platform_driver_register()


#define DRV_NAME			"keyint"

#define	KEY_MATRIX_BASE1	0
#define	KEY_MATRIX_BASE2	1	
#define	KEY_MATRIX_BASE3	2	
#define	KEY_MATRIX_BASE4	3	
#define	KEY_MATRIX_BASE5	4	

#define EXAMPLE				100 
//======================================
// 100 : workqueue test
// 200 : tasklet test
// others : non bottom-half
//======================================

#if 1
	#define gprintk(fmt, x... ) printk( "%s: " fmt, __FUNCTION__ , ## x)
#else
	#define gprintk(x...) do { } while (0)
#endif

static int key_major = 0, key_minor = 0;
static int result;
static dev_t key_dev = 0;
static struct cdev key_cdev;

static char dev_name1[] = "mc100_keyscan";
static int cur_key, old_key;
static flag = 0;

static DECLARE_WAIT_QUEUE_HEAD(wq);

struct ldd_device{
	char			*name;
	unsigned short	vendor;
	struct device	dev;
};

#define to_ldd_device(dev) container_of(dev, struct ldd_device, dev);

struct mc100_key_detection
{
    int             irq;
    int             pin;
    int             pin_setting;
    char            *name;
    int             last_state;
#if (EXAMPLE == 100)
	struct work_struct gdetect;
#elif (EXAMPLE == 200)
	struct tasklet_struct gdetect;
#endif
};

static struct mc100_key_detection mc100_gd = {
    IRQ_EINT10, S5PC1XX_GPH1(2), S5PC1XX_GPH1_2_WAKEUP_INT_10, "key-detect", 0
};

static int scan_input(void);
static int key_register_cdev(void);

//#define to_ldd_device(dev) container_of(dev, struct ldd_device, dev);

static irqreturn_t
mc100_keyevent(int irq, void *dev_id, struct pt_regs *regs)
{
    struct mc100_key_detection *gd = (struct mc100_key_detection *) dev_id;
    int             state;

	state = 1;
	printk("gd= %x, keypad was pressed \n",gd);

    if (!gd)
        return IRQ_HANDLED;

#if 1
    state = gpio_get_value(S5PC1XX_GPH1(2));

    gd->last_state = state;

    gprintk("%s gd %s\n\n", gd->name, state ? "high" : "low");
#endif

#if (EXAMPLE == 100)
	schedule_work(&gd->gdetect);
#elif (EXAMPLE == 200)
	tasklet_schedule(&gd->gdetect);
#endif 

	flag = 1;
	wake_up_interruptible(&wq);

	return IRQ_HANDLED;

}

static int scan_input(void) {

	if(((readl(S5PC1XX_GPH1DAT) >> 2) & 0x1) != 0x1)
	{
		//if(!mc100_gpio_getpin(S3C2410_GPF3))
		if(!gpio_get_value(S5PC1XX_GPH1(2)))
			return 2;
	}
	return 0;
}

#if (EXAMPLE == 100)
static void mc100_keyint_callback(void *pgd)
{
    struct mc100_key_detection *gd = (struct mc100_key_detection *)pgd;
    
    int state = gd->last_state;

	gprintk("workqueue callback call\n\n");
}
#elif (EXAMPLE == 200)
static void mc100_keyint_callback(ulong data)
{
    struct mc100_key_detection *gd = (struct mc100_key_detection *)data;
    
    int state = gd->last_state;

	int i;

	gprintk("tasklet callback call\n");

}
#endif

static ssize_t mc100_keyscan_read(struct file *filp, char *buff, size_t count, loff_t *offp)
{
	int i;
	int key_base[5] = {KEY_MATRIX_BASE1, KEY_MATRIX_BASE2, KEY_MATRIX_BASE3, KEY_MATRIX_BASE4, KEY_MATRIX_BASE5};

	cur_key = 0;

#if 0
	interruptible_sleep_on(&wq);
#else
	wait_event_interruptible(wq, flag != 0);
	//wait_event_interruptible_timeout(wq, flag != 0, 600);
#endif
	
#if	KEY_DEBUG
	printk("key input\n");
#endif

	// change input port
	s3c_gpio_cfgpin(S5PC1XX_GPH1(2), S3C_GPIO_INPUT);
	for(i=3; i>=0; i--)
	{
		writel(readl(S5PC1XX_GPH1DAT) | (0x3), S5PC1XX_GPH1DAT);
		writel(readl(S5PC1XX_GPH1DAT) & (~(0x1 << i % 2)), S5PC1XX_GPH1DAT);
		
		if(cur_key = scan_input())
		{
			cur_key += key_base[i];
			if(cur_key == old_key)
				return 0;
			old_key = cur_key;
			put_user(cur_key,(char *)buff);
			break;
		}
	}
	
	old_key = 0;	
	flag = 0;

	s3c_gpio_setpin(S5PC1XX_GPH1(0), 0);
	s3c_gpio_setpin(S5PC1XX_GPH1(1), 0);
	// set GPBDAT 0
	
	// change External Interrupts
	s3c_gpio_cfgpin(S5PC1XX_GPH1(2), S3C_GPIO_SFN(2));

	return count;
}

static int mc100_keyscan_open(struct inode * inode, struct file * file)
{
	old_key = 0;

	printk(KERN_INFO "ready to scan key value\n");

	return 0;
}

static int mc100_keyscan_release(struct inode * inode, struct file * file)
{
	printk(KERN_INFO "end of the scanning\n");

	return 0;
}

static struct file_operations mc100_keyscan_fops = {
	.owner		= THIS_MODULE,
	.open		= mc100_keyscan_open,
	.release	= mc100_keyscan_release,
	.read		= mc100_keyscan_read,
};

static int mc100keypad_remove(struct platform_device *pdev)
{
	free_irq(mc100_gd.irq, &mc100_gd);
		
#if (EXAMPLE == 100)

#elif (EXAMPLE == 200)
	tasklet_kill(&mc100_gd.gdetect);
#endif

	printk("this is mc100keypad_remove\n");
	return 0;
}

static int __init mc100keypad_probe(struct platform_device *pdev)
{
	int ret;

	// set output mode
	s3c_gpio_cfgpin(S5PC1XX_GPH1(0), S3C_GPIO_OUTPUT);
	s3c_gpio_cfgpin(S5PC1XX_GPH1(1), S3C_GPIO_OUTPUT);

	// set data
	s3c_gpio_setpin(S5PC1XX_GPH1(0), 0);
	s3c_gpio_setpin(S5PC1XX_GPH1(1), 0);

	s3c_gpio_cfgpin(S5PC1XX_GPH1(2), S3C_GPIO_SFN(2));
	writel(readl(S5PC1XX_EINT1CON) & (~(0x7 << 8)), S5PC1XX_EINT1CON);
	writel(readl(S5PC1XX_EINT1CON) | (0x2 << 8), S5PC1XX_EINT1CON); // Falling Edge int

	s3c_gpio_setpull(S5PC1XX_GPH1(0), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PC1XX_GPH1(1), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PC1XX_GPH1(2), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PC1XX_GPH1(3), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PC1XX_GPH1(4), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PC1XX_GPH1(5), S3C_GPIO_PULL_NONE);
	
	if(request_irq(IRQ_EINT10, mc100_keyevent, IRQF_DISABLED, DRV_NAME, &mc100_gd) )     
    {
		printk("failed to request external interrupt.\n");
        ret = -ENOENT;
        return ret;
    }
#if (EXAMPLE == 100)
	INIT_WORK(&mc100_gd.gdetect, mc100_keyint_callback);
#elif (EXAMPLE == 200)
	tasklet_init(&mc100_gd.gdetect, mc100_keyint_callback, (unsigned long)(&mc100_gd)); 
#endif

    printk("this is mc100keypad_probe\n");

	
	return 0;
}

/*** bus registeration ****/
///*

//*/
#if 0
static int ldd_match(struct device *dev, struct device_driver *driver)
{
	return !strncmp(dev->bus_id, driver->name, strlen(driver->name));
}
#endif

struct bus_type ldd_bus_type = {
	.name = "ldd",
//	.match = ldd_match,
//	.hotplug = ldd_hotplug,
};

static void pdev_release(struct device * dev){
//	dev->parent 	= NULL;
	printk("pdev_release done\n");
}
//static void release_pdev(struct device * dev){
static void ldd_bus_release(struct device * dev){
	printk(KERN_DEBUG "ldd_bus release\n");
}
struct device ldd_bus = {
	.bus_id = "ldd0",
	.release	= ldd_bus_release,

};

#if 1 //platform_device
static struct ldd_device pdev  =
{
	.name = "mc100-keypad",
	.vendor = 100,
	.dev	= {
//		.bus		= &ldd_bus_type,
//		.parent		= &ldd_bus, 
//		.bus_id		= "mc100-keypad",
//		.release	= pdev_release,
	},
};
#else
static struct ldd_device *ldddev = {
	.name = "mc100-keypad",
	.dev = ldd_bus,
};

/*
static struct device pdev  =
{
	.bus_id	= "kkk",
	.bus	= &platform_bus_type,
	.release	= release_pdev,	
};
*/
#endif

#if 0 //platform_driver
static struct platform_driver mc100keypad_driver = {
       .driver         = {
	       .name   = "mc100-keypad",
	       .owner  = THIS_MODULE,
       },
       .probe          = mc100keypad_probe,
       .remove         = mc100keypad_remove,
};
#else //device_driver
static struct device_driver mc100keypad_driver = {
	   .name   = "mc100-keypad",
	   .owner  = THIS_MODULE,
	  // .bus		= &platform_bus_type,
	   .bus			= &ldd_bus_type,
       .probe          = mc100keypad_probe,
       .remove         = mc100keypad_remove,
};
#endif


//static struct class_device key_cdevice;
/*
static struct class_device key_cdevice = {
	.class = &key_class,
//	.devt = key_dev,
	.class_id	= "keypad0",
};
*/

static void key_class_release(struct class *dev)
{
	printk("key class release\n"); 
}

static struct class key_class = {
	.name	="key",
	.class_release = key_class_release,
};
//static struct class_device cdevice_key;

// POINT:
int ldd_device_register(struct ldd_device *ldddev)
{
	ldddev->dev.bus = &ldd_bus_type;
	ldddev->dev.parent = &ldd_bus;
	ldddev->dev.release = pdev_release;
	strncpy(ldddev->dev.bus_id, ldddev->name, BUS_ID_SIZE);
	return device_register(&ldddev->dev);
}
EXPORT_SYMBOL(ldd_device_register);

void ldd_device_unregister(struct ldd_device *ldddev)
{
	device_unregister(&ldddev->dev);
}
EXPORT_SYMBOL(ldd_device_unregister);


static int __init mc100_keyint_init(void)
{
	//key_class = class_create(THIS_MODULE, "key");
	//if(IS_ERR(key_class))
	//	return PTR_ERR(key_class);
	
	key_register_cdev();

	/*** bus register ***/
	result = bus_register(&ldd_bus_type);
	if(result)
		return result;
	

	//class_device_add(&cdevice_key);
	class_register(&key_class);
///*
//	memset(&key_cdevice, 0, sizeof(key_cdevice));
//	key_cdevice.class = &key_class;
	
	
	//key_cdevice.devt = key_dev;

	//key_cdevice.class_id = "key0";
//	strlcpy(key_cdevice.class_id, "keypad0", KOBJ_NAME_LEN);
//	printk("DBG devt = 0x%x \n", key_cdevice.devt);
//*/
	//class_device_register(&key_cdevice);

	
	//result = platform_driver_register(&mc100keypad_driver);
	result = driver_register(&mc100keypad_driver);
	#if 1
	
	if(!result){
		printk("platform_driver initiated  = %d \n", result);
		//result = platform_device_register(&pdev);
		//strlcpy(pdev.bus_id, "aaa",BUS_ID_SIZE);
		
		result = device_register(&ldd_bus);
		printk("1. platform_device_result = %d \n", result);

		//strncpy(pdev.dev.bus_id, pdev.name, BUS_ID_SIZE);
		//result = device_register(&pdev.dev);	
		result = ldd_device_register(&pdev);

		printk("2. platform_device_result = %d \n", result);
		if(result)
			//platform_driver_unregister(&mc100keypad_driver);
			driver_unregister(&mc100keypad_driver);
	}
	#endif
	
	/*
	ldddev->dev.bus	= &ldd_bus_type;
	ldddev->dev.parent	= &ldd_bus;
	ldddev->dev.release = ldd_dev_release;
	strncpy(ldddev->dev.bus_id, ldddev->name, BUS_ID_SIZE);
	return device_register(&ldddev->dev);
	*/
	printk(KERN_INFO "%s successfully loaded\n", DRV_NAME);

	return result;
    
}

static void __exit mc100_keyint_exit(void)
{
    
	cdev_del(&key_cdev);
	unregister_chrdev_region(key_dev, 1);

	//device_register(&ldddev.dev);
#if 1
	//platform_device_unregister(&pdev);
	device_unregister(&pdev.dev);
	device_unregister(&ldd_bus);
#endif
	//platform_driver_unregister(&mc100keypad_driver);
	driver_unregister(&mc100keypad_driver);
	
	//class_device_unregister(&key_cdevice);
	//class_destroy(key_class);
	class_unregister(&key_class);

	bus_unregister(&ldd_bus_type);

    printk(KERN_INFO "%s successfully removed\n", DRV_NAME);
}
#if 1
static int key_register_cdev(void)
{
    int error;
	
	/* allocation device number */
	if(key_major) {
		key_dev = MKDEV(key_major, key_minor);
		error = register_chrdev_region(key_dev, 1, dev_name1);
	} else {
		error = alloc_chrdev_region(&key_dev, key_minor, 1, dev_name1);
		key_major = MAJOR(key_dev);
	}
	
	if(error < 0) {
		printk(KERN_WARNING "keyscan: can't get major %d\n", key_major);
		return result;
	}
	printk("major number=%d\n", key_major);
	
	/* register chrdev */
	cdev_init(&key_cdev, &mc100_keyscan_fops);
	key_cdev.owner = THIS_MODULE;
	error = cdev_add(&key_cdev, key_dev, 1);
	
	if(error)
		printk(KERN_NOTICE "Keyscan Register Error %d\n", error);
	
	return 0;
}
#endif

module_init(mc100_keyint_init);
module_exit(mc100_keyint_exit);

MODULE_AUTHOR("Jongin,Cha <jongin@mdstec.com>");
MODULE_LICENSE("GPL");
