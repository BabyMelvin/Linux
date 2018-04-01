#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h> 
#include <asm/uaccess.h>  //jiffies

MODULE_LICESNE("GPL");
MODULE_AUTHOR("Melvin");
MODULE_DESCRIPTION("timer module");
MODULE_ALIAS("timer module");

struct timer_list timer;

static void timer_function(int para){
	printk("timer expired and para is %s\n",para);
}

static __init int timer_init(){
	init_timer(&timer);
	timer.data=5;
	timer.expires=jiffies+(5*HZ);
	timer.function=timer_function;
	add_timer(&timer);
	
	return 0;
}

static __exit void timer_exit(){
	del_timer(&timer);
}
module_init(timer_init);
module_exit(timer_exit);