#include <linux/module.h>
#include <linux/init.h>

/**
* insmod param.ko
* insmod hello.ko name="yasuo" age=10
*/
MODULE_LICENSE("GPL");
MODULE_AUTHORE("Melvin");
MODULE_DESCRIPTION("Hello world module");
//参数
static char* name="hang";
static int age=30;
module_param(age,int,S_IRUGO);
module_param(name,charp,S_IRUGO);

//文件外函数

/*
	查看模块: cat /proc/kallsyms
*/
extern int add_integer(int,int);
extern int sub_integer(int,int);
static int __init hello_init(void){
	/*
		图形化界面printk权限要求为KERN_EMMERG
			KERN_ERR:打印输出保存在/var/log/message中.
	*/
	printk(KERN_ERR "hello world\n");
	printk(KERN_EMERG "NAME :%s\n",name);
	printk(KERN_EMERG "age:%d\n",age);
	
	int res=add_integer(1,2);
	printk(KERN_EMERG "hello init,res=%d\n",res);
	return 0;
}

static void __exit hello_exit(){
	printk(KERN_EMERG “hello exit!”);
	int res=sub_integer(2,1);
	printk(KERN_EMERG "hello exit,res=%d\n",res);
}

module_init(hello_init);
module_exit(hello_exit);