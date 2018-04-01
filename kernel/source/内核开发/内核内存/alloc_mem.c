#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Melvin");
MODULE_DESCRIPTION("memory allo module");
MODULE_ALIAS("malloc memeory");

char *buf1=NULL;
char *buf2=NULL;

static int __init alloc_init(){
	buf1=kmalloc(100,GFP_KERNEL);
	memset(buf1,0,100);
	strcpy(buf1,"<<<---Kmalloc mem ok! ---->>>");
	printk("buf1:%s",buf1);
	
	buf2=get_zeroedd_page(GFP_KERNEL);
	strcpy(buf2,"<<<--- get free page ok!---->>>");
	printk("buf2:%s\n",buf2);
	return 0;
}

static void __exit alloc_exit(){
	kfree(buf1);
	free_page(buf2);
	printk("<<<<---- module exit ---->>>");
}

module_init(alloc_init);
module_exit(alloc_exit);