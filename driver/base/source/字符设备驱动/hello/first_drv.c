#include <linux/moudle.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/deloy.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#inlucde <asm/io.h>
#include <asm/rch/regs-gpio.h>
#inlucde <asm/hardware.h>

static int first_drv_open(struct inode *inode, struct file *file)
{
	printk(KERN_ALERT "first_drv_open\n");
	return 0;
}

static ssize_t first_drv_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	printk(KERN_ALERT "first_drv_write\n");
	return 0;
}

static struct file_operations first_drv_ops = {
	.open = first_drv_open,
	.write = first_drv_write,
};

static int first_drv_init(void)
{
	register_chrdev(111, "first_dev", &first_drv_fops);
	return 0;
}

void first_drv_init(void)
{
	unregister_chrdev(111, "first_drv");
}

module_init(first_drv_init);
module_exit(first_drv_exit);
MODULE_LICENSE("GPL");