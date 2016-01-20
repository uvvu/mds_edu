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
#include <linux/workqueue.h> //work queue

#include <linux/cdev.h> //cdev_init
#include <linux/wait.h> //wait_event_interruptible
#include <asm/uaccess.h> //user access
#include <linux/fs.h> //file_operatios 

#include <linux/platform_device.h> //platform_driver_register()

#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <plat/regs-gpio.h>
#include <plat/gpio-bank-h1.h>

#include <linux/input.h> //input_dev

#define DRV_NAME			"keyint"
#define S3C2410BUTVERSION	0x001


#define	KEY_MATRIX_BASE1	0
#define	KEY_MATRIX_BASE2	1
#define	KEY_MATRIX_BASE3	2
#define	KEY_MATRIX_BASE4	3
#define	KEY_MATRIX_BASE5	4

#define EXAMPLE 100 
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
//static int flag = 0;

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
	{IRQ_EINT10, S5PC1XX_GPH1(2), S5PC1XX_GPH1_2_WAKEUP_INT_10, "key-detect", 1},
	{IRQ_EINT11, S5PC1XX_GPH1(3), S5PC1XX_GPH1_3_WAKEUP_INT_11, "key-detect", 1}, 
	{IRQ_EINT12, S5PC1XX_GPH1(4), S5PC1XX_GPH1_4_WAKEUP_INT_12, "key-detect", 1}, 
	{IRQ_EINT13, S5PC1XX_GPH1(5), S5PC1XX_GPH1_5_WAKEUP_INT_13, "key-detect", 1}
};

static struct mc100_buttons_private
{
	struct input_dev *input;
	spinlock_t		lock;
	int 			count;
	int 			shift;
	char 			phys[32];
};

struct mc100_buttons_private * mc100_buttons_private;

static int scan_input(void);
//static int key_register_cdev(void);

static irqreturn_t
mc100_keyevent(int irq, void *dev_id, struct pt_regs *regs)
{
    struct mc100_key_detection *gd = (struct mc100_key_detection *) dev_id;
    int             state;

	int i, cur_key, old_key;
	state = 1;
	printk("gd= %x, keypad was pressed \n",(unsigned int)gd);

    if (!gd)
        return IRQ_HANDLED;

	#if 1
    state = s3c_gpio_getvaule(gd->pin);

    gd->last_state = state;

    gprintk("%s gd %s\n\n", gd->name, state ? "high" : "low");
	#endif

	//for key scan
	#if 1
	cur_key = 0;
	s3c_gpio_cfgpin(S5PC1XX_GPH1(2), S3C_GPIO_INPUT);
	s3c_gpio_cfgpin(S5PC1XX_GPH1(3), S3C_GPIO_INPUT);
	s3c_gpio_cfgpin(S5PC1XX_GPH1(4), S3C_GPIO_INPUT);
	s3c_gpio_cfgpin(S5PC1XX_GPH1(5), S3C_GPIO_INPUT);

	for(i=4; i>=0; i--)
	{
		writel(readl(S5PC1XX_GPHDAT) | (0x1f), S5PC1XX_GPHDAT);
		writel(readl(S5PC1XX_GPHDAT) & (~(0x1 << i)), S5PC1XX_GPHDAT);
		
		cur_key = scan_input();
		if(cur_key)
		//cur_key = scan_input();
		{
			cur_key += (i)*2;//key_base[i];
			if(cur_key == old_key)
				goto SameValue;
			old_key = cur_key;
			printk("cur_key = %d \n\n", cur_key);
			
			//add by june
			//printk("private->input = 0x%x, cur_key=%d \n", mc100_buttons_private->input, cur_key);
			input_report_key(mc100_buttons_private->input, cur_key, 1);
			//mdelay(100);
			input_sync(mc100_buttons_private->input);

			input_report_key(mc100_buttons_private->input, cur_key, 0);
			//mdelay(100);
			input_sync(mc100_buttons_private->input);



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

	#if (EXAMPLE == 100)
	schedule_work(&gd->gdetect);
	#elif (EXAMPLE == 200)
	tasklet_schedule(&gd->gdetect);
	#endif 

	return IRQ_HANDLED;

}

static int scan_input(void) {

	if(((readl(S3C2410_GPFDAT) >> 3) & 0x1) != 0x1)
	{
		if(!s3c2410_gpio_getpin(S3C2410_GPF3))
			return 2;
	}
	return 0;
}

#if (EXAMPLE == 100)
static void mc100_keyint_callback(void *pgd)
{
	int i;
	gprintk("workqueue callback call\n\n");

	//for key scan
	#if 0
	cur_key = 0;
	s3c_gpio_cfgpin(S3C2410_GPF3, S3C2410_GPF3_INP);
	for(i=4; i>=0; i--)
	{
		writel(readl(S5PC1XX_GPHDAT) | (0x1f), S5PC1XX_GPHDAT);
		writel(readl(S5PC1XX_GPHDAT) & (~(0x1 << i)), S5PC1XX_GPHDAT);
		
		cur_key = scan_input();
		if(cur_key)
		//cur_key = scan_input();
		{
			cur_key += (i)*2;//key_base[i];
			if(cur_key == old_key)
				goto SameValue;
			old_key = cur_key;
			printk("cur_key = %d \n\n", cur_key);
			
			//add by june
			//printk("private->input = 0x%x, cur_key=%d \n", mc100_buttons_private->input, cur_key);
			input_report_key(mc100_buttons_private->input, cur_key, 1);
			//mdelay(100);
			input_sync(mc100_buttons_private->input);

			input_report_key(mc100_buttons_private->input, cur_key, 0);
			//mdelay(100);
			input_sync(mc100_buttons_private->input);



			//put_user(cur_key,(char *)buff);
			break;
		}
	}
SameValue:
	old_key = 0;

	// set GPBDAT 0
	s3c_gpio_setpin(S5PC1XX_GPH0, 0);
	s3c_gpio_setpin(S5PC1XX_GPH1, 0);
	s3c_gpio_setpin(S5PC1XX_GPH2, 0);
	s3c_gpio_setpin(S5PC1XX_GPH3, 0);
	s3c_gpio_setpin(S5PC1XX_GPH4, 0);
	
	// change External Interrupts
	s3c_gpio_cfgpin(S3C2410_GPF3, S3C2410_GPF3_EINT3);
	#endif
}
#elif (EXAMPLE == 200)
static void mc100_keyint_callback(ulong data)
{
    //struct mc100_key_detection *gd = (struct mc100_key_detection *)data;    
    //int state = gd->last_state;
	int i;
	//int key_base[5] = {KEY_MATRIX_BASE5, KEY_MATRIX_BASE4, KEY_MATRIX_BASE3, KEY_MATRIX_BASE2, KEY_MATRIX_BASE1};
	

	gprintk("tasklet callback call\n");


	//for key scan
	#if 0
	cur_key = 0;
	s3c_gpio_cfgpin(S3C2410_GPF3, S3C2410_GPF3_INP);
	for(i=4; i>=0; i--)
	{
		writel(readl(S5PC1XX_GPHDAT) | (0x1f), S5PC1XX_GPHDAT);
		writel(readl(S5PC1XX_GPHDAT) & (~(0x1 << i)), S5PC1XX_GPHDAT);
		
		cur_key = scan_input();
		if(cur_key)
		//cur_key = scan_input();
		{
			cur_key += (i)*2;//key_base[i];
			if(cur_key == old_key)
				goto SameValue;
			old_key = cur_key;
			printk("cur_key = %d \n\n", cur_key);
			
			//add by june
			//printk("private->input = 0x%x, cur_key=%d \n", mc100_buttons_private->input, cur_key);
			input_report_key(mc100_buttons_private->input, cur_key, 1);
			//mdelay(100);
			input_sync(mc100_buttons_private->input);

			input_report_key(mc100_buttons_private->input, cur_key, 0);
			//mdelay(100);
			input_sync(mc100_buttons_private->input);



			//put_user(cur_key,(char *)buff);
			break;
		}
	}
SameValue:
	old_key = 0;

	// set GPBDAT 0
	s3c_gpio_setpin(S5PC1XX_GPH0, 0);
	s3c_gpio_setpin(S5PC1XX_GPH1, 0);
	s3c_gpio_setpin(S5PC1XX_GPH2, 0);
	s3c_gpio_setpin(S5PC1XX_GPH3, 0);
	s3c_gpio_setpin(S5PC1XX_GPH4, 0);
	
	// change External Interrupts
	s3c_gpio_cfgpin(S3C2410_GPF3, S3C2410_GPF3_EINT3);
	#endif
}
#endif




static int mc100_keypad_remove(struct platform_device *pdev)
{
	free_irq(mc100_gd.irq, &mc100_gd);
		
	#if (EXAMPLE == 100)
	#elif (EXAMPLE == 200)
	tasklet_kill(&mc100_gd.gdetect);
	#endif
	struct mc100_buttons_private *mc100_buttons_private_temp = platform_get_drvdata(pdev);
	input_unregister_device(mc100_buttons_private_temp->input);
	kfree(mc100_buttons_private_temp);

	printk("this is mc100_keypad_remove\n");
	return 0;
}

static int __init mc100_keypad_probe(struct platform_device *pdev)
{
	int i;
	int ret;
	int error;
	struct input_dev 	*input_dev;
	unsigned int gpio, end;

	end = S5PC1XX_GPH1(6);

	for(gpio=S5PC1XX_GPH1(0);gpio<end;gpio++) {
		if (gpio < S5PC1XX_GPH1(2)) {
			s3c_gpio_cfgpin(gpio, S3C_GPIO_OUTPUT);
			s3c_gpio_setpin(gpio, 0);
		}
		else
			s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(2));

		s3c_gpio_setpull(gpio, S3C_GPIO_PULL_NONE);
	}

	writel(readl(S5PC1XX_EINT1CON) & (~(0x7 << 8)), S5PC1XX_EINT1CON);
	writel(readl(S5PC1XX_EINT1CON) & (~(0x7 << 12)), S5PC1XX_EINT1CON);
	writel(readl(S5PC1XX_EINT1CON) & (~(0x7 << 16)), S5PC1XX_EINT1CON);
	writel(readl(S5PC1XX_EINT1CON) & (~(0x7 << 20)), S5PC1XX_EINT1CON);
	writel(readl(S5PC1XX_EINT1CON) | (0x2 << 8), S5PC1XX_EINT1CON); // Falling E
	writel(readl(S5PC1XX_EINT1CON) | (0x2 << 12), S5PC1XX_EINT1CON); // Falling
	writel(readl(S5PC1XX_EINT1CON) | (0x2 << 16), S5PC1XX_EINT1CON); // Falling
	writel(readl(S5PC1XX_EINT1CON) | (0x2 << 20), S5PC1XX_EINT1CON); // Falling
	writel(readl(S5PC1XX_EINT1MASK) & (~(0x1 << 2)), S5PC1XX_EINT1MASK); // wake
	writel(readl(S5PC1XX_EINT1MASK) & (~(0x1 << 3)), S5PC1XX_EINT1MASK); // wake
	writel(readl(S5PC1XX_EINT1MASK) & (~(0x1 << 4)), S5PC1XX_EINT1MASK); // wake
	writel(readl(S5PC1XX_EINT1MASK) & (~(0x1 << 5)), S5PC1XX_EINT1MASK); // wake
	s3c_gpio_setpull(S5PC1XX_GPH1(0), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PC1XX_GPH1(1), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PC1XX_GPH1(2), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PC1XX_GPH1(3), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PC1XX_GPH1(4), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S5PC1XX_GPH1(5), S3C_GPIO_PULL_NONE);
				 
	//input device register
	mc100_buttons_private = kzalloc(sizeof(struct mc100_buttons_private), GFP_KERNEL);	
	input_dev = input_allocate_device();

	if(!mc100_buttons_private || !input_dev){
		printk("memory alloc error --------------------------\n");
		error = -ENOMEM;
		goto fail;
	}
	
	platform_set_drvdata(pdev, mc100_buttons_private);
	mc100_buttons_private->input = input_dev;
	input_dev->evbit[0] = BIT(EV_KEY);
	input_dev->private	= mc100_buttons_private;

	input_dev->name 	= DRV_NAME;
	input_dev->id.bustype	=	BUS_HOST;
	input_dev->id.vendor 	= 	0xDEAD;
	input_dev->id.product	= 	0xBEEF;
	input_dev->id.version 	= 	S3C2410BUTVERSION;

	for(i=10;i>0;i--){
		set_bit(i,input_dev->keybit);
	}

	if( request_irq(IRQ_EINT10, mc100_keyevent, IRQF_DISABLED, DRV_NAME, &mc100_gd) )     
    {
                printk("failed to request external interrupt.\n");
                ret = -ENOENT;
               return ret;
    }
	#if (EXAMPLE == 100)
	//INIT_WORK(&mc100_gd.gdetect, mc100_keyint_callback, &mc100_gd);
	INIT_WORK(&mc100_gd.gdetect, mc100_keyint_callback);
	#elif (EXAMPLE == 200)
	tasklet_init(&mc100_gd.gdetect, mc100_keyint_callback, (unsigned long)(&mc100_gd)); 
	#endif

	input_register_device(input_dev);
    printk("this is mc100_keypad_probe\n");

	
	return 0;

fail:	kfree(mc100_buttons_private);
	input_free_device(input_dev);
	return error;
}

static void release_pdev(struct device * dev){
	dev->parent 	= NULL;
}

static struct platform_device pdev  =
{
	.name	= "mc100-keypad",
	.id		= -1,
	.dev	= {
		.release	= release_pdev,
	},
};

static struct platform_driver mc100_keypad_driver = {
       .driver         = {
	       .name   = "mc100-keypad",
	       .owner  = THIS_MODULE,
       },
       .probe          = mc100_keypad_probe,
       .remove         = mc100_keypad_remove,
};

static int __init mc100_keyint_init(void)
{
	int result;
	result = platform_driver_register(&mc100_keypad_driver);
	
	if(!result){
		printk("platform_driver initiated  = %d \n", result);
		result = platform_device_register(&pdev);
		printk("platform_device_result = %d \n", result);
		if(result)
			platform_driver_unregister(&mc100_keypad_driver);
	}
	printk(KERN_INFO "%s successfully loaded\n", DRV_NAME);

	return result;
    
}

static void __exit mc100_keyint_exit(void)
{
	platform_device_unregister(&pdev);
	platform_driver_unregister(&mc100_keypad_driver);

    printk(KERN_INFO "%s successfully removed\n", DRV_NAME);
}

module_init(mc100_keyint_init);
module_exit(mc100_keyint_exit);

MODULE_AUTHOR("Jongin, Cha <jongin@mdstec.com>");
MODULE_LICENSE("GPL");


