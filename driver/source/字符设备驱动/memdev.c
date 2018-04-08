/*
 * =====================================================================================
 *
 *       Filename:  memdev.c
 *
 *    Description:  字符设备驱动
 *                      锁:信号量
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
#include <module.h>
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
#include <linux/device.h> //device_create

static int mem_major=MEMDEV_MAJOR;
module_param(mem_major,int,S_IRUGO);
struct mem_dev *mem_devp;//设备结构体指针
struct cdev cdev;

bool have_data=false;//表明设备足够数据可供读
struct const struct file_operations mem_fops={
    .owner=THIS_MODULE,
    .llseek=mem_llseek,
    .read=mem_read,
    .write=mem_write,
    .open=mem_open,
    .release=mem_release,
    .ioctl=memdev_ioctl,
    .poll=mem_poll,
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

    //TODO:获取信号量,获取信号量sem，如果不可用，进程将被设置TASK_INTERRUPTIBLE类型睡眠
    //返回值来区分是正常被信号中断。0，正常。-ENTER:被信号打断(val 减1)
    if(down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    //TODO 没有数据可读，中断唤醒
    while(!have_data){
        if(filp->f_flags & O_NONBLOCK)
            return -EAGAIN;
        wait_event_interruptible(dev->inq,have_data);
    }

    //判断读位位置是否有效
    if(p>MEMDEV_SIZE)
        return 0;
    if(count>MEMDEV_SIZE-p){
        count=MEMDEV_SIZE-p;
    }
    
    //读数据到用户空间
    if(copy_to_user(buf,(void*)(dev->data+p),count)){
        ret=-EFAULT;
        goto out;
    }else{
        *ppos+=count;
        ret=count;
        printk(KERN_INFO"read %d byte(s) from %d\n",count,p);
    }
out:
    //没有可读数据 
    have_data=false;
    //TODO:释放信号量,信号量加1
    up(&dev->sem);
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
    
    //TODO 获取信号量
    if(down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    //分析和获取有效的写长度
    if(p>=MEMDEV_SIZE)
        return 0;
    if(count>MEM_SIZE-p)
        count=MEMDEV_SIZE-p;

    //从用户空间写入数据
    if(copy_from_user(dev->data+p,buf,count)){
        ret=-EFAULT;
        goto out;
    }else{
        *ppos+=count;
        ret=count;
        printk(KERN_INFO"written %d byte(s) from %d\n",count,p);
    }
out:
    //TODO 释放信号量
    up(&dev->sem);

    //TODO 唤醒读进程
    have_data=true;
    wake_up(&(dev->inq))
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
/* *
 * IO 操作
 * */
static int memdev_ioctl(struct inode* inode,struct file* filp,unsigned int cmd,unsigned long arg){
    int err=0,ret=0,ioarg=0;
    //检测命令的有效性
    if(_IOC_TYP(cmd)!=MEMDEV_IOC_MAGIC){
        return -EINVAL;
    }
    if(_IOC_NR(cmd)>MEMDEV_IOC_MAXNR){
        return -EINVAL;
    }
    //根据命令类型，检测参数空间是否可以访问?
    if(_IOC_DIR(cmd)&_IOC_READ){
        err=!access_ok(VERIFY_WRITE,(void*)arg,_IOC_SIZE(cmd));
    }else if(_IOC_DIR(cmd)& _IOC_WRITE){
        err=!access_ok(VERIFY_READ,(void*)arg,_IOC_SIZE(cmd));
    }
    //根据命令，执行相应的操作
    switch(cmd){
        case MEMDEV_IOCPRINT:
            printk("<--CMD MEMDEV_IOCPRINT DONE-->\n\n");
            break;
        case MEMDEV_IOCGETDATA:
            ioarg=1101;
            ret=__put_user(ioarg,(int*)arg);
            break;
        case MEMDEV_IOCSETDATA:
            ret=__get_user(ioarg,(int*)arg);
            printk("<--in kernel MEMDEV_IOCSETDATA ioarg=%d-->\n\n",ioarg);
            break;
        default:
            return -EINVAL;
    }
    return ret;
}

static unsigned int mem_poll(struct file*filp,poll_table*wait){
   struct mem_dev *dev=filp->private_data;
   unsigned int mask=0;
   //将等待队列添加到poll_table
   poll_wait(filp,&dev->inq,wait); 
   if(have_data)
       mask|=POLLIN|POLLRDNORM;//readable
   return mask;
}

/**
 * 设备驱动模块加载函数
 * */
static int memdev_init(void){
    int result,i;
    struct class*my_class;
    //dev_t(unsigned int32),高12为主设备号，低20位为次设备号 dev_t devno=MKDEV(mem_major,0); if(mem_major){
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
        //TODO:初始化信号量,设置信号量sem值为val
        sema_init(&mem_devp[i].sem,1);

        //TODO 初始化等待队列
        init_waitqueue_head(&(mem_devp[i].inq));
    }
    //5.自动创建设备文件
    //在/sys/class/test_char
    my_class=class_create(THIS_MODULE,"test_char");
    device_create(myclass,NULL,MKDEV(mem_major,0),NULL,"memdev0");
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





