/*
 * =====================================================================================
 *
 *       Filename:  memdev.c
 *
 *    Description:  字符设备驱动
 *
 *        Version:  1.0
 *        Created:  04/08/2018 02:18:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include "memdev.h"

static int mem_major=MEMDEV_MAJOR;
module_param(mem_major,int,S_IRUGO);
struct mem_dev *mem_devp;//设备结构体指针
struct cdev cdev;

struct const struct file_operations mem_fops={
    .owner=THIS_MODULE,
    .llseek=mem_llseek,
    .read=mem_read,
    .write=mem_write,
    .open=mem_open,
    .release=mem_release,
};

/** 
 * 文件打开函数
 * */
static int mem_open(struct inode* inode,struct file* filp){
   struct mem_dev* dev; 
   //获取此设备号
   int num=MINOR(inode->i_rdev);

   if(num>=MEMDEV_NR_DEVS)
       return -ENODEV;

   dev=&mem_devp[num];

   //将设备描述结构指针赋值给文件私有数据指针
   filp->private_data=dev;
   return 0;
}

/**
 * 文件释放
 * */
static int mem_release(struct inode* inode,struct file* filp){
    return 0;
}

/**
 * 读函数
 * */
static ssize_t mem_read(struct file* filp,char __user* buf,size_t size,loff_t * ppos){
    unsigned long p=*ppos;
    unsigned int count=size;

    int ret=0;
    //获得设备结构指针
    struct mem_dev* dev=filp->private;
    //判断读位位置是否有效
    if(p>MEMDEV_SIZE)
        return 0;
    if(count>MEMDEV_SIZE-p){
        count=MEMDEV_SIZE-p;
    }
    
    //读数据到用户空间
    if(copy_to_user(buf,(void*)(dev->data+p),count)){
        ret=-EFAULT;
    }else{
        *ppos+=count;
        ret=count;
        printk(KERN_INFO"read %d byte(s) from %d\n",count,p);
    }
    return ret;
}

/**
 * 写函数
 * */
static ssize_t mem_write(struct file*filp,const char __user *buf,size_t size,loff_t*ppos){
    unsigned long p=*ppos;
    unsigned int count=size;
    int ret=0;
    struct mem_dev *dev=filp->private_data;
    
    //分析和获取有效的写长度
    if(p>=MEMDEV_SIZE)
        return 0;
    if(count>MEM_SIZE-p)
        count=MEMDEV_SIZE-p;

    //从用户空间写入数据
    if(copy_from_user(dev->data+p,buf,count)){
        ret=-EFAULT;
    }else{
        *ppos+=count;
        ret=count;
        printk(KERN_INFO"written %d byte(s) from %d\n",count,p);
    }
    return ret;
}

/**
 * seek 文件定位
 * */
static loff_t mem_llseek(struct file*filp,loff_t offset,int whence){
    loff_t newpos;
    switch(whence){
        case SEEK_SET:
            newops=offset;
            break;
        case SEEK_CUR:
            newpos=filp->f_ops+offset;
            break;
        case SEEK_END:
            newpos=MEMDEV_SIZE-1+offset;
            break;
        default:
            return -EINVAL;
    }
    if(newpos<0||newpos>MEMDEV_SIZE){ 
        return -EINVAL;
    }
    filp->f_pos=newpos;
    return newpos;
}
/**
 * 设备驱动模块加载函数
 * */
static int memdev_init(void){
    int result,i;
    //dev_t(unsigned int32),高12为主设备号，低20位为次设备号
    dev_t devno=MKDEV(mem_major,0);
    if(mem_major){
        //静态注册，from devno,count=2,name="memdev"->/proc/devices
        result=register_chrdev_region(devno,2,"memdev");
    }else{
        //动态分配，(dev,baseminor,count,name)
        result=alloc_chrdev_region(&devno,0,2,"memdev");
        //从dev_t分解主设备号
        mem_major=MAJOR(devno);
    }
    if(result<0){
        return result;
    }
    //1.初始化cdev结构
    cdev_init(&cdev,&mem_fops);
    cdev.owner=THIS_MODULE;
    cdev.ops=&mem_fops;

    //2.注册字符设备
    cdev_add(&cdev,MKDEV(mem_major,0),MEMDEV_NR_DEVS);

    //3.为设备描述结构分配内存
    mem_devp=kmalloc(MEMDEV_NR_DEVS*sizeof(struct mem_dev),GFP_KERNEL);
    if(!mem_devp){
        //分配失败
        result=-ENOMEM;
        goto fail_malloc;
    }
    memset(mem_devp,0,sizeof(struct mem_dev));    

    //4.为设备结构各自分配内存
    for(i=0;i<MEMDEV_NR_DEVS;i++){
        mem_devp[i].size=MEMDEV_SIZE;
        mem_devp[i].data=kmalloc(MEMDEV_SIZE,GFP_KERNEL);
        memset(mem_devp[i].data,0,MEMDEV_SIZE); 
    }
    return 0;
fail_malloc:
    unregister_chrdev_region(devno,1);
    return result;
}

/**
 * 模块卸载函数
 * */
static void memdev_exit(void){
    cdev_del(&cdev);//注销设备
    kfree(mem_devp);//释放设备结构体内存
    unregister_chrdev_region(MKDEV(mem_major,0),2); //释放设备号
}
MODULE_AUTHOR("Melvin");
MODULE_LICENSE("GPL");

module_init(memdev_init);
module_exit(memdev_exit);





