/****************************************
 *	Filename: mmapcall_dev.c
 *	Title: MMAP Test Driver
 *	Desc:
 ****************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>

#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <asm/page.h>
#include <linux/mm.h>

#define	MMAPCALL_DEV_NAME		"mmapcall"
#define	MMAPCALL_DEV_MAJOR		240

MODULE_LICENSE("Dual BSD/GPL");

static int mmapcall_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int mmapcall_release(struct inode *inode, struct file *filp)
{
	return 0;
}

// POINT: 
static int mmapcall_mmap(struct file *filp, struct vm_area_struct *vma)
{

	// TODO: print mmap information


	return -EAGAIN;
}

struct file_operations mmapcall_fops =
{
	.owner		= THIS_MODULE,
	.open		= mmapcall_open,
	.release	= mmapcall_release,
	.mmap		= mmapcall_mmap,
};

static int mmapcall_init(void)
{
	int result;

	result = register_chrdev(MMAPCALL_DEV_MAJOR, MMAPCALL_DEV_NAME, &mmapcall_fops);
	if(result < 0)	return result;
	printk(KERN_INFO "Register Character Device Major Number: %d\n", MMAPCALL_DEV_MAJOR);

	return 0;
}

static void mmapcall_exit(void)
{
	unregister_chrdev(MMAPCALL_DEV_MAJOR, MMAPCALL_DEV_NAME);
}

module_init(mmapcall_init);
module_exit(mmapcall_exit);


