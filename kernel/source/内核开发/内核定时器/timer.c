#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h> 
#include <asm/uaccess.h>  //jiffies

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Melvin");
MODULE_DESCRIPTION("timer module");
MODULE_ALIAS("timer module");

struct timer_list timer;

static void timer_function(int para){
    printk(KERN_ERR"melvin function = %s\n", __func__);
	printk("melvin timer expired and para is %d\n",para);
}

static __init int timer_init(void){
    printk(KERN_ERR"melvin function = %s\n", __func__);
	init_timer(&timer);
	timer.data= 10;
	timer.expires = jiffies + (5 * HZ);
	timer.function = timer_function;
	add_timer(&timer);
	
	return 0;
}

static __exit void timer_exit(void){
    printk(KERN_ERR"melvin function = %s\n", __func__);
	del_timer(&timer);
}
module_init(timer_init);
module_exit(timer_exit);
