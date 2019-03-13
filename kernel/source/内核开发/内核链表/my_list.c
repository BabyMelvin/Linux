#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Melvin");
MODULE_DESCRIPTION("list module");

struct student{
	char name[100];
	int num;
	struct list_head list;
};

struct student *pstudent;
struct student *tmp_student;
struct list_head student_list;
struct list_head *pos;

static __init int mylist_init(void){
	int i=0;
    printk(KERN_ERR"melvin function = %s\n", __func__);

	INIT_LIST_HEAD(&student_list);
	pstudent = kmalloc(sizeof(struct student) * 5, GFP_KERNEL);
	memset(pstudent, 0, sizeof(struct student) * 5);
	for(i = 0; i < 5; i++){
		sprintf(pstudent[i].name, "student %d", i + 1);
		pstudent[i].num = i + 1;
		list_add(&(pstudent[i].list), &student_list);
	}
	
	list_for_each(pos, &student_list){
		tmp_student = list_entry(pos, struct student, list);
		printk(KERN_ERR"melvin student %d name :%s\n", tmp_student->num, tmp_student->name);
	}

	return 0;
}

static __exit void mylist_exit(void){
	int i;
    printk(KERN_ERR"melvin function = %s\n", __func__);
	for(i=0; i < 5; i++){
		list_del(&(pstudent[i].list));
	}
	kfree(pstudent);
}

module_init(mylist_init);
module_exit(mylist_exit);
