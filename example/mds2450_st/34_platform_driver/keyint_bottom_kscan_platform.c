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
#include <linux/workqueue.h> //work queue

#include <linux/workqueue.h> //work queue

#include <linux/cdev.h> //cdev_init
#include <linux/wait.h> //wait_event_interruptible
#include <asm/uaccess.h> //user access
#include <linux/fs.h> //file_operatios 

#include <linux/platform_device.h> //platform_driver_register()

#include <linux/input.h> //input_dev

#define DRV_NAME		"keyint"
#define S3C2410BUTVERSION	0x001


#define	KEY_MATRIX_BASE1	0
#define	KEY_MATRIX_BASE2	1
#define	KEY_MATRIX_BASE3	2
#define	KEY_MATRIX_BASE4	3
#define	KEY_MATRIX_BASE5	4

#define EXAMPLE 200 
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

//static int key_major = 0, key_minor = 0;
//static int result;
//static dev_t key_dev;
//static struct cdev key_cdev;

//static char dev_name[] = "mc100_keyscan";
static int cur_key, old_key;
static flag = 0;

static DECLARE_WAIT_QUEUE_HEAD(wq);

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
//static int key_register_cdev(void);

static irqreturn_t
mc100_keyevent(int irq, void *dev_id, struct pt_regs *regs)
{
    struct mc100_key_detection *gd = (struct mc100_key_detection *) dev_id;
    int state;

	state = 1;
	printk("gd= %x, keypad was pressed \n",(unsigned int)gd);

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
		if(!gpio_get_value(S5PC1XX_GPH1(2)))
			return 2;
	}
	return 0;
}

#if (EXAMPLE == 100)
static void mc100_keyint_callback(void *pgd)
{
    struct mc100_key_detection *gd = (struct mc100_key_detection *)pgd    
    int state = gd->last_state;
	int i;

	gprintk("workqueue callback call\n\n");

	//for key scan
#if 1
	cur_key = 0;
	s3c_gpio_cfgpin(S5PC1XX_GPH1(2), S3C_GPIO_INPUT);
	for(i=3; i>=0; i--)
	{
		writel(readl(S5PC1XX_GPH1DAT) | (0x3), S5PC1XX_GPH1DAT);
		writel(readl(S5PC1XX_GPH1DAT) & (~(0x1 << i % 2)), S5PC1XX_GPH1DAT);
		
		cur_key = scan_input();
		if(cur_key)
		//cur_key = scan_input();
		{
			cur_key += key_base[i];
			if(cur_key == old_key)
				return 0;	
			old_key = cur_key;
			printk("cur_key = %d \n\n", cur_key);
			
			//put_user(cur_key,(char *)buff);
			break;
		}
	}
SameValue:
	old_key = 0;

	// set GPBDAT 0
	s3c_gpio_setpin(S5PC1XX_GPH1(0), 0);
	s3c_gpio_setpin(S5PC1XX_GPH1(1), 0);
	
	// change External Interrupts
	s3c_gpio_cfgpin(S5PC1XX_GPH1(2), S3C_GPIO_SFN(2));
	#endif
}
#elif (EXAMPLE == 200)
static void mc100_keyint_callback(ulong data)
{
    struct mc100_key_detection *gd = (struct mc100_key_detection *)data;    
    int state = gd->last_state;

	int key_base[5] = {KEY_MATRIX_BASE5, KEY_MATRIX_BASE4, KEY_MATRIX_BASE3, KEY_MATRIX_BASE2, KEY_MATRIX_BASE1};
	int i;

	gprintk("tasklet callback call\n");

	//for key scan
#if 1
	cur_key = 0;
	s3c_gpio_cfgpin(S5PC1XX_GPH1(2), S3C_GPIO_INPUT);
	for(i=4; i>=0; i--)
	{
		writel(readl(S5PC1XX_GPH1DAT) | (0x3), S5PC1XX_GPH1DAT);
		writel(readl(S5PC1XX_GPH1DAT) & (~(0x1 << i%2)), S5PC1XX_GPH1DAT);
		
		cur_key = scan_input();
		if(cur_key)
		//cur_key = scan_input();
		{
			cur_key += key_base[i];
			if(cur_key == old_key)
				return 0;	
			old_key = cur_key;
			printk("cur_key = %d \n\n", cur_key);
			
			//put_user(cur_key,(char *)buff);
			break;
		}
	}
SameValue:
	old_key = 0;

	// set GPBDAT 0
	s3c_gpio_setpin(S5PC1XX_GPH1(0), 0);
	s3c_gpio_setpin(S5PC1XX_GPH1(1), 0);
	
	// change External Interrupts
	s3c_gpio_cfgpin(S5PC1XX_GPH1(2), S3C_GPIO_SFN(2));
#endif
}
#endif

// POINT: 하드웨어 제거용 
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

// POINT: 하드웨어 검출용
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
	writel(readl(S5PC1XX_EINT1CON) | (0x2 << 8), S5PC1XX_EINT1CON); // Fallin
	
	s3c_gpio_setpull(S5PC1XX_GPH1(0), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PC1XX_GPH1(1), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PC1XX_GPH1(2), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PC1XX_GPH1(3), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PC1XX_GPH1(4), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PC1XX_GPH1(5), S3C_GPIO_PULL_NONE);

	if(request_irq(IRQ_EINT10, mc100_keyevent, IRQF_DISABLED, DRV_NAME, &mc100_gd))
    {
		printk("failed to request external interrupt.\n");
		ret = -ENOENT;
		return ret;
    }
#if (EXAMPLE == 100)
	INIT_WORK(&mc100_gd.gdetect, mc100_keyint_callback, &mc100_gd);
#elif (EXAMPLE == 200)
	tasklet_init(&mc100_gd.gdetect, mc100_keyint_callback, (unsigned long)(&mc100_gd)); 
#endif

    printk("this is mc100keypad_probe\n");
	
	return 0;
}

static void release_pdev(struct device * dev)
{
	dev->parent 	= NULL;
}

// POINT: 플랫폼 디바이스 초기화 
static struct platform_device pdev  =
{
	.name	= "keypad",
	.id		= -1,
	.dev	= {
		.release	= release_pdev,
	},
};

// TODO: 플랫폼 드라이버 객체 mc100keypad_driver 초기화 



static int __init mc100_keyint_init(void)
{
	int result;
	
	//TODO: 플랫폼 드라이버 등록 
	
	if(!result){
		printk("platform_driver initiated  = %d \n", result);

		// TODO: 플랫폼 디바이스 등록 
		
		
		printk("platform_device_result = %d \n", result);
		if(result)
			platform_driver_unregister(&mc100keypad_driver);
	}
	printk(KERN_INFO "%s successfully loaded\n", DRV_NAME);

	return result;
    
}

static void __exit mc100_keyint_exit(void)
{
	
	// TODO: 플랫폼 디바이스 해제

	// TODO: 플랫폼 드라이버 해제


    printk(KERN_INFO "%s successfully removed\n", DRV_NAME);
}

module_init(mc100_keyint_init);
module_exit(mc100_keyint_exit);

MODULE_AUTHOR("Jongin, Cha <jongin@mdstec.com>");
MODULE_LICENSE("GPL");
