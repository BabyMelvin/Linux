#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>

/* *
 * 1.#insmod bus.ko       加载总线
 * 2.#insmod device.ko    在总线上添加设备
 * 3.#insmod driver.ko    为总线上设备添加驱动
 *
 * a.首先调用driver_register来实现驱动注册，
 * b.驱动注册函数中内核会在相应的my_bus总线上去遍历所有的设备，做匹配操作
 *      匹配函数为my_match(bus.c)，匹配方式为设备名(device.c中my_dev.bus_id)和驱动注册的名字(device.c中my_driver.name)是否一致,匹配成功，说明驱动能驱动设备，去驱动函数my_probe(driver.c)被调用，打印"Driver found device which my driver can handle!"
 *  c.除了注册驱动，函数my_driver_init还调用函数driver_create_file创建驱动属性文件
 *      /sys/bus/my_bus/drivers/my_dev/drv
 *
 * # ls /sys/bus/my_bus/drivers
 *      my_dev
 * # ls /sys/bus/my_bus/drivers/my_dev/
 *      bind drv my_dev uevent unbind
 *  TODO 总结:设备模型:总线、设备、驱动;设备和驱动都是挂载在总线上
 *            总线上可以先有设备后有驱动，也可以先有驱动后有设备。
 * */
MODULE_AUTHOR("Melvin");
MODULE_LICENSE("Dual BSD/GPL");

extern struct bus_type my_bus_type;

struct device_driver my_driver = {
	.name = "my_dev",
	.bus = &my_bus_type,
	.probe = my_probe,
    .remove	= my_remove,
};
/*
 * Export a simple attribute.
 */
static ssize_t mydriver_show(struct device_driver *driver, char *buf)
{
	return sprintf(buf, "%s\n", "This is my driver!");
}


//TODO cat /sys/bus/my_bus/drivers/my_dev/drv
static DRIVER_ATTR(drv, S_IRUGO, mydriver_show, NULL);
static int my_probe(struct device *dev)
{
    printk("Driver found device which my driver can handle!\n");
    return 0;
}

static int my_remove(struct device *dev)
{
    printk("Driver found device unpluged!\n");
    return 0;
}

static int __init my_driver_init(void)
{
	int ret = 0;
        
    /*注册驱动*/
	driver_register(&my_driver);
		
	/*创建属性文件*/
	driver_create_file(&my_driver, &driver_attr_drv);
	
	return ret;	

}
static void my_driver_exit(void)
{
	driver_unregister(&my_driver);
}
module_init(my_driver_init);
module_exit(my_driver_exit);
