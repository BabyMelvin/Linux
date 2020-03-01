#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>


/* *
 * ls /sys/bus/my_bus/devices/
 *      my_dev
 *
 * ls /sys/devices/my_bus0/
 *      my_dev   uevent
 * ls /sys/devices/my_bus0/my_dev/
 *      dev      subsystem  uevent
 * */
MODULE_AUTHOR("Melvin");
MODULE_LICENSE("GPL");

extern struct device my_bus;
extern struct bus_type mybus_type;

static void my_dev_relase(struct device*dev){
}

//TODO my_bus 和 my_bus_type来自  bus.c文件
//将会建立 /sys/bus/my_bus/devices/my_dev  (bus_id)
//和建立   /sys/devices/my_bus0/my_dev
struct device my_dev={
	.bus=&my_bus_type,
	.parent=&my_bus,
	.release=my_dev_release,
};
//export a simple attribute
static ssize_t mydev_show(struct device*dev,struct device_attribute*attr){
	return sprintf(buf,"%s\n","this a my device!");
}

// TODO 建立属性 dev
// cat /sys/devices/my_bus0/my_dev/dev: 结果 this is my device!
static DEVICE_ATTR(dev,S_IRUGO,mydev_show,NULL);
static __init int my_device_init(void){
	int ret=0;
	//初始化设备
	strncpy(my_dev.bus_id,"my_dev",BUS_ID_SIZE);
	//注册设备
	device_register(&my_dev);
	//创建属性文件
	device_create_file(&my_dev,&dev_attr_dev);
	return ret;
}
static void my_device_exit(void){
	device_unregister(&my_dev);
}
module_init(my_device_init);
module_exit(my_device_exit);
