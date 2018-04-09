#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/stat.h>

MODULE_AUTHOR("Melvin");
MODULE_LICENSE("Dual BSD/GPL");
void obj_test_release(struct kobject*kobject);
ssize_t kobj_test_show(struct kobject*kobject,struct attribute*attr,char*buf);
ssize_t kobj_test_store(struct kobject*kobject,struct attribute*attr,const char*buf,size_t count);


ssize_t kobj_test_show(struct kobject*kobject,struct attribute*attr,char*buf){
	printk("have show.\n");
	printk("attrname:%s\n",attr->name);
	sprintf(buf,"%s\n",attr->name);
	return strlen(attr->name)+2;
}
ssize_t kobj_test_store(struct kobject*kobject,struct attribute*attr,const char *buf,size_t count){
	printk("have store\n");
	printk("write:%s\n",buf);
	return count;
}
void obj_test_release(struct kobject*kobject){
	printk("eric_test:release\n");
}

struct attribute test_attr={
	.name="kobj_config",
	.mode=S_IRWXUGO,
};
struct attribute *def_attrs[]={
	&test_attr,
	NULL,
};
struct sysfs_ops obj_test_sysops={
	.show=kobj_test_show,
	.store=kobj_test_shore,
};
struct kobj_type ktype={
	.release=obj_test_release,
	.sysfs_ops=&obj_test_sysops,
	.default_attrs=def_attrs,
};
struct kobject kobj;
static int kobj_test_init(void){
	printk("kobject test init\n");
	kobject_init_and_add(&kobj,ktype,NULL,"kobject_test");
	return 0;
}
module_init(kobj_test_init);
module_init(kobj_test_exit);