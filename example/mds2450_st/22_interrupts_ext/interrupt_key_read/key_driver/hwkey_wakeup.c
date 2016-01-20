/* ********************************************
 * hwkey_wakeup.c 
 * Desc: Syncronized the user process.
 * - sleep and wakeup process using wait queue
 * ********************************************/
#include <linux/module.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <linux/bitops.h>
#include <linux/capability.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/in.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/notifier.h>
#include <linux/miscdevice.h>

#include <linux/irq.h>
#include <asm/io.h>
#include <asm/irq.h>

#include <mach/map.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/regs-gpio.h>
#include <plat/irqs.h>

#include <linux/slab.h>
#include <linux/ktime.h>
#include <linux/delay.h>

#define IRQ_KEY_UEVENT	IRQ_EINT(16)

#define GET_TIMEOFDAY 		1
#define GET_EXECUTIONTIME 	2
#define IOCTL_READ_WRITE	_IOWR(0xA1, 0x01, int)

MODULE_AUTHOR("Youngdeok");
MODULE_LICENSE("GPL");

// POINT: define the wait queue
// TODO: eventq이름으로 대기큐 선언 


static int key_press_counter = 0;

static irqreturn_t hwkey_isr(int irq, void *dev_id)
{
    printk("[hwkey] interrupt occurred\n");
    key_press_counter++;
	// POINT: send the event message to userspace
	// blocked read() systemcall will be wake up!
	// TODO: 유저영역에 블럭된 프로그램을 깨운다. 
	// wake_up_interruptible을 이용한다. 

	
    return IRQ_HANDLED;
}

int hwkey_open(struct inode *inode, struct file *filp)
{
	int ret = -ENODEV;
	
 	if( !try_module_get(THIS_MODULE) )
    	goto out;	

    ret = 0;
    
out:	
	return ret;    
}

int hwkey_release(struct inode *inode, struct file *filp)
{
	module_put(THIS_MODULE);
    return 0;
}

ssize_t hwkey_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	int val = 0;
	
    if (count < sizeof(int))
        return -EINVAL;

    if (signal_pending(current)) {
        return -ERESTARTSYS;
    }
    
    val = key_press_counter; // read value
    
	// POINT: sleep the user process which want to read something 
	// from the driver
	// TODO: 버퍼 데이터가 준비되기 전까지 잠을 재우도록 sleep 시킨다.
	// interruptible_sleep_on을 사용한다. 
	
    
    if (copy_to_user(buf, (void *)&val, sizeof(int))) {
        return -EFAULT;
    }
    return sizeof(int);
}

int parse_data(unsigned int cmd, void __user *arg)
{
    int ret;
    struct timeval tv;
	time_t cursec;
	ktime_t start, end;
	u32 actual_time;

	int data;

    // copy the data from the userspace to kernelspace
    ret = copy_from_user(&data, arg, sizeof(int));
    if(ret) return -EFAULT;

    switch(data)
    {
        case GET_TIMEOFDAY:
            
            do_gettimeofday(&tv);
            cursec = tv.tv_sec;

            printk(KERN_INFO "do_gettimeofday:%lld\n", 
				(long long)cursec);
            break;

        case GET_EXECUTIONTIME:
			start = ktime_get();
			mdelay(100); // do something
			end = ktime_get();
			actual_time = ktime_to_ns(ktime_sub(end,start));
			printk(KERN_INFO "Time taken for execution: %lld\n", 
				(long long)actual_time);        
            break;

        default:
            ret = -EFAULT;
            break;
    }

    if( ret != -EFAULT ) {
        ret = copy_to_user(arg, &data, sizeof(int));
    }

    if(ret) ret = -EFAULT;
    else    ret = 0;

    return ret;
}

long hwkey_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{    
	int ret = 0;

    switch (cmd)
    {
	    case IOCTL_READ_WRITE:
	    	ret = parse_data( cmd, (void __user *)arg );
	    	break;
	    default:    	
			printk(KERN_ERR "there is no command\n");
	        ret = -ENOIOCTLCMD;
    }
    
    return ret;
}

ssize_t hwkey_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

void hwkey_gpio_set(void)
{
	/* Push Key Button */
	s3c_gpio_cfgpin(EXYNOS4_GPX2(0), S3C_GPIO_SFN(0xf));
	s3c_gpio_setpull(EXYNOS4_GPX2(0), S3C_GPIO_PULL_UP);
}

struct file_operations key_fops = {
  .owner    = THIS_MODULE,
  .open     = hwkey_open,
  .read		= hwkey_read,
  .unlocked_ioctl = hwkey_ioctl,
  .release  = hwkey_release,
};

struct miscdevice key_miscdev = {
    MISC_DYNAMIC_MINOR, "hwkey", &key_fops
};

static int __init hwkey_init(void)
{
	int ret = 0;
	
	hwkey_gpio_set();
	
	ret = request_irq(IRQ_KEY_UEVENT, hwkey_isr, IRQF_TRIGGER_FALLING, "keytest", NULL);	
	if (ret) {
        printk("failed to request interrupt.\n");
        ret = -ENOENT;
        goto err;
    }
	
	ret = misc_register(&key_miscdev);
	if( ret<0 ) {
		printk(KERN_ERR "misc driver register error\n");
		goto err;
	}
	
	// TODO: 대기큐를 초기화 한다. 
	// init_waitqueue_head를 사용한다. 
	

    printk("[key_driver] init.\n");

	return 0;
err: 
    return ret;
}

static void __exit hwkey_exit(void)
{
	misc_deregister(&key_miscdev);
	free_irq(IRQ_KEY_UEVENT, NULL);	
	printk("[key_driver] exit.\n");
}

module_init(hwkey_init);
module_exit(hwkey_exit);

