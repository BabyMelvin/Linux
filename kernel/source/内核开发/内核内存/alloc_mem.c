#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Melvin");
MODULE_DESCRIPTION("memory allo module");
MODULE_ALIAS("malloc memeory");

char* buf1 = NULL;
char* buf2 = NULL;
unsigned long page_;
static int __init alloc_init(void)
{
    buf1 = kmalloc(100, GFP_KERNEL);
    memset(buf1, 0, 100);
    strcpy(buf1, "<<<---Kmalloc mem ok! ---->>>");
    printk(KERN_ERR "melvin buf1:%s", buf1);

    page_ = get_zeroed_page(GFP_KERNEL);
    buf2 = (char*)page_;
    strcpy(buf2, "<<<--- get free page ok!---->>>");
    printk(KERN_ERR "melvin buf2:%s\n", buf2);
    return 0;
}

static void __exit alloc_exit(void)
{
    kfree(buf1);
    free_page(page_);
    printk(KERN_ERR "melvin <<<<---- module exit ---->>>");
}

module_init(alloc_init);
module_exit(alloc_exit);
