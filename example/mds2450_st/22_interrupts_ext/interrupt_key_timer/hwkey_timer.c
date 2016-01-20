/* ********************************************
 * hwkey_timer.c 
 * Desc: key dectection example.
 * - key detect using timer handler
 * ********************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/device.h>

#include <linux/irq.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <asm/irq.h>

#include <mach/map.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/regs-gpio.h>
#include <plat/irqs.h>

#include <linux/slab.h>

#define REPEAT_DELAY	HZ/10
static struct timer_list gadget_check_timer;

#define DRV_NAME 		"hwkey"
#define IRQ_KEY_UEVENT	IRQ_EINT(16)

struct hwkey_detection {
    int		irq;
    int     pin;
    int     pin_setting;
    char    *name;
    int     last_state;
};

static struct hwkey_detection hwkey_detect = {
    IRQ_KEY_UEVENT , EXYNOS4_GPX2(0), 0xff, "key-detect", 0
};

#ifdef CONFIG_HOTPLUG

char *hotplug_path = "/sbin/keyevent";

// kernel fail의 가능성이 있는 코드는 irq routine에서 불리지 못한다.
static void hwkey_detect_hotplug(char *verb)
{
	char *argv[3], **envp, *buf, *scratch;
	int i = 0, j, value;

	if (!hotplug_path[0]) {
		printk(KERN_ERR "gadget: calling hotplug without a hotplug agent defined\n");
		return;
	}
	
	#if 1
	if (in_interrupt()) {
		printk(KERN_ERR "gadget: calling hotplug from interrupt\n");
		return;
	}
	#endif
	
	if (!(envp = (char **) kmalloc(20 * sizeof(char *), GFP_KERNEL))) {
		printk(KERN_ERR "input.c: not enough memory allocating hotplug environment\n");
		return;
	}
	if (!(buf = kmalloc(1024, GFP_KERNEL))) {
		kfree (envp);
		printk(KERN_ERR "gadget: not enough memory allocating hotplug environment\n");
		return;
	}

	argv[0] = hotplug_path;
	argv[1] = "gadget";
	argv[2] = NULL;

	envp[i++] = "HOME=/";
	envp[i++] = "PATH=/sbin:/bin:/usr/sbin:/usr/bin";

	scratch = buf;

	envp[i++] = scratch;
	scratch += sprintf(scratch, "ACTION=%s", verb) + 1;

	envp[i++] = NULL;


	printk("gadget detect: calling %s %s [%s %s %s]\n",
		argv[0], argv[1], envp[0], envp[1], envp[2] );

	value = call_usermodehelper(argv [0], argv, envp, 0);

	kfree(buf);
	kfree(envp);

	if (value != 0)
		printk("gadget detect: hotplug returned %d\n", value);

}

#endif

static irqreturn_t hwkey_isr(int irq, void *dev_id)
{
    struct hwkey_detection *gd = (struct hwkey_detection *) dev_id;
    int state = 1;

    if (!gd)
        return IRQ_HANDLED;

    gd->last_state = state;

    printk("%s gd %s\n", gd->name, state ? "high" : "low");
    
    #ifdef CONFIG_HOTPLUG
   	mod_timer(&gadget_check_timer, jiffies + REPEAT_DELAY);
    #endif

    return IRQ_HANDLED;
}

static void timer_callback(unsigned long data)
{
    struct hwkey_detection *gd = (struct hwkey_detection *) data;
    
    int state = gd->last_state;

	if( state )
		hwkey_detect_hotplug("add");
	else
		hwkey_detect_hotplug("remove");

}

void hwkey_gpio_set(void)
{
	/* Push Key Button */
	s3c_gpio_cfgpin(EXYNOS4_GPX2(0), S3C_GPIO_SFN(0xf));
	s3c_gpio_setpull(EXYNOS4_GPX2(0), S3C_GPIO_PULL_UP);
}


int __init hwkey_init(void)
{
	int ret;
	
	hwkey_gpio_set();
    
	// TODO: gadget_check_timer변수로 타이머를 초기화하고
	// timer_callback 함수를 핸들러로 등록한다. 
	// 사용할 data는 hwkey_detect를 사용한다. 
    init_timer(&gadget_check_timer);
	gadget_check_timer.function = timer_callback;
	gadget_check_timer.expires = 
		get_jiffies_64() + REPEAT_DELAY; 
	gadget_check_timer.data = 
		(unsigned long)&hwkey_detect;
	add_timer(&gadget_check_timer);

	ret = request_irq(IRQ_KEY_UEVENT, hwkey_isr, 
				IRQF_TRIGGER_FALLING, "hwkey_timer", &hwkey_detect);
	if (ret) {
		printk("request_irq failed (IRQ_EINT16)!\n");
		ret = -EIO;
		return ret;
	}

    printk(KERN_INFO "%s init.\n", DRV_NAME);
    return 0;
}

void hwkey_exit(void)
{
    free_irq(hwkey_detect.irq, &hwkey_detect);
    del_timer(&gadget_check_timer);
    
    printk(KERN_INFO "%s exit.\n", DRV_NAME);
}


module_init(hwkey_init);
module_exit(hwkey_exit);


MODULE_AUTHOR("Youngdeok");
MODULE_DESCRIPTION("detect module test");
MODULE_LICENSE("GPL");
