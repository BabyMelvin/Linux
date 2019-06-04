#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/io.h>

#include "memdev.h"

#define DEVICE_NAME "leds"

static unsigned long led_table[]={
	S3C2410_GPB5,
	S3C2410_GPB6,
	S3C2410_GPB7,
	S3C2410_GPB8,
};
static unsigned int led_cfg_table[]={
	S3C2410_GPB5_OUTP,
	S3C2410_GPB6_OUTP,
	S3C2410_GPB7_OUTP,
	S3C2410_GPB8_OUTP,
};
static int sbc2440_leds_ioctl(struct inode*inode,struct file*file,unsigned int cmd,unsigned long arg){
	int i=0;
	//检测命令的有效性
	if(_IO_TYPE(cmd)!=MEMDEV_IOC_MAGIC){
		return -EINVAL;
	}
	if(_IOC_NR(cmd)>MEMDEV_IOC_MAXNR)
		return -EINVAL;
	//根据命令，执行相应的操作
	switch(cmd){
		case MEMDEV_IOCOFF:
			cmd=1;
			for(i=0;i<4;i++){
				s3c2440_gpio_setpin(led_table[i],cmd);
			}
			return 0;
			break;
		case MEMDEV_IOCON:
			for(i=0;i<4;i++){
				s3c2440_gpio_setpin(led_table[i],!cmd);
			}
			break;
		default:
			return -EINVAL;
	}
}
//文件结构体
static struct file_operations dev_fops={
	.owner=THIS_MODULE,
	.ioctl=sbc2440_leds_ioctl,
};
static struct miscdevice misc={
	.minor=MISC_DYNAMIC_MINOR,
	.name=DEVICE_NAME,
	.fops=&dev_fops,
};
static int __init dev_init(void){
	int ret;
	int i;
	//设置GPIO控制寄存器，GPIO设置为输出模式，默认下灯全灭
	for(i=0;i<4;i++){
		s3c2440_gpio_cfgpin(led_table[i],led_cfg_table[i]);
		s3c2440_gpio_setpin(led_table[i],1);
	}
	//注册混杂型字符设备驱动
	ret=misc_register(&misc);
	return ret;
}
static void __exit dev_exit(void){
	//注销混杂型字符设备驱动
	misc_deregister(&misc);
}
module_init(dev_init);
module_exit(dev_exit);
MODULE_AUTHOR("Melvin");
MUDULE_LICENSE("GPL");
