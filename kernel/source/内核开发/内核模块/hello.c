#include <linux/init.h>
#include <linux/module.h>

static int age = 30;
static char* name = "caohang";
module_param(age, int, S_IRUGO);
module_param(name, charp, S_IRUGO);

extern int add_integer(int, int);
extern int sub_integer(int, int);
static int __init hello_init(void)
{
    int res = 0;
    printk(KERN_ERR "hang hello world\n");
    res = add_integer(1, 2);
    printk(KERN_EMERG "hang hello init,res = %d\n", res);
    printk(KERN_ERR "hang name = %s,age = %d\n", name, age);
    return 0;
}

static void __exit hello_exit(void)
{
    int res = 0;
    printk(KERN_EMERG "hang hello exit");
    res = sub_integer(2, 1);
    printk(KERN_EMERG "hang hello exit,res= %d\n", res);
}

module_init(hello_init);
module_exit(hello_exit);
