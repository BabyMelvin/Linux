#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
int add_integer(int a, int b)
{
    return a + b;
}

int sub_integer(int a, int b)
{
    return a - b;
}

static int __init sym_init(void)
{
    printk(KERN_ERR "hang sym_init");
    return 0;
}

static void __exit sym_exit(void)
{
    printk(KERN_ERR "hang sym_exit");
}

module_init(sym_init);
module_exit(sym_exit);

EXPORT_SYMBOL(add_integer);
EXPORT_SYMBOL(sub_integer);
