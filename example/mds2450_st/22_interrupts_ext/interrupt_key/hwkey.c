/* *************************************
 * hwkey.c 
 * Desc: request_irq function test.
 * - make Interrupt Service Routine
 * *************************************/
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


#define DRV_NAME "hwkey"

#define IRQ_KEY_UEVENT	IRQ_EINT(16)

// POINT: Interrupt Service Routine
// TODO: ISR�� ����� hwkey_isr �Լ� �ۼ�
static irqreturn_t hwkey_isr(int irq, void *dev_id)
{
	unsigned long flags;
	local_irq_save(flags);
	local_irq_disable();

	printk("[hwkey] interrupt occurred\n");

    local_irq_restore(flags);
	return IRQ_HANDLED;
}

void hwkey_gpio_set(void)
{
	/* POINT: Push Key Button */
	s3c_gpio_cfgpin(EXYNOS4_GPX2(0), S3C_GPIO_SFN(0xf));
	s3c_gpio_setpull(EXYNOS4_GPX2(0), S3C_GPIO_PULL_UP);
}


int __init hwkey_init(void)
{
	int ret;

	hwkey_gpio_set();
	
	// POINT: register the ISR to catch the key event
	// TODO: IRQ_KEY_UEVENT�� ISR ���
	// IRQF_TRIGGER_FALLING���� ���ͷ�Ʈ�� ����Ѵ�. 
	ret = request_irq(
		IRQ_KEY_UEVENT,  // IRQ Number 
		hwkey_isr, 		 // ISR Handler Name 
		IRQF_TRIGGER_FALLING, // Flags
		"keytest",       // debug name
		NULL);           // dev_id

	if (ret) {
		printk("request_irq failed (IRQ_EINT13)!\n");
		ret = -EIO;
		return ret;
	}

	printk(KERN_INFO "%s device init() called\n", DRV_NAME);

    return 0;
}

void hwkey_exit(void)
{
	// POINT: free the IRQ
	// TODO: ���ͷ�Ʈ ��� ����
	free_irq(IRQ_KEY_UEVENT, NULL);
    printk(KERN_INFO "%s device exit() called\n", DRV_NAME);
}


module_init(hwkey_init);
module_exit(hwkey_exit);


MODULE_AUTHOR("Youndeok");
MODULE_DESCRIPTION("key detect driver");
MODULE_LICENSE("GPL");


