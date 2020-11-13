/*
 * =====================================================================================
 *
 *       Filename:  memdev.c
 *
 *    Description:  mmap映射
 *
 *        Version:  1.0
 *        Created:  04/08/2018 06:41:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <stdlib.h>

#include "memdev.h"
#include <linux/kernel.h>

static int mem_major = MEMDEV_MAJOR;
module_param(mem_major, int, S_IRUGO);
struct mem_dev* mem_devp; //设备结构体指针

struct cdev cdev;

static const struct file_opeations mem_fops = {
    .owner = THIS_MODULE,
    .open = mem_open,
    .release = mem_release,
    .mmap = memdev_mmap,
};

static int mem_open(struct inode* inode, struct file* filp)
{
    struct mem_dev* dev;
    int num = MINOR(inode->i_rdev);
    if (num >= MEMDEV_NR_DEVS) {
        return -ENODEV;
    }
    filp->private_data = dev;
    return 0;
}
static int mem_release(struct inode* inode, struct file* filp)
{
    return 0;
}

//TODO 一个进程的内存区域可以通过查看/proc/pid/maps
/* *
 * vm_area_struct{
 *     vm_start,
 *     vm_end,
 *     vm_flags, 
 * }
 * */
static int memdev_mmap(struct filp* filp, struct vm_area_struct* vma)
{
    struct mem_dev* dev = filp->priavte_data;
    vma->vm_flags |= VM_IO;
    vma->vm_flags |= VM_RESERVED;

    //一次建立所有页表(vma,addr,pfn,size,prot)->
    //(虚拟内存区域指针，虚拟内存其实起始值，要映射的物理地址所在物理页帧,大小，保护属性)
    if (remap_pfn_range(vma, vma->vm_start, virt_to_phys(dev->data) >> PAGE_SHIFT, vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
        return -EAGAIN;
    }
    return 0;
}
//加载模块函数
static int memdev_init(void)
{
    int result, i;
    dev_t devno = MKDEV(mem_major, 0);
    if (mem_major) {
        result = register_chrdev_region(devno, 2, "memdev");
    } else {
        result = alloc_chrdev_region(&devno, 0, 2, "memdev");
        mem_major = MAJOR(devno);
    }
    if (result < 0) {
        return result;
    }
    //初始化结构体
    cdev_init(&cdev, &mem_fops);
    cdev.owner = THIS_MODULE;
    cdev.ops = &mem_fops;
    //注册字符设备
    cdev_add(&cdev, MKDEV(mem_major, 0), MEMDEV_NR_DEVS);

    //为设备描述结构体分配内存
    mem_devp = kmalloc(MEMDEV_NR_DEVS * sizeof(struct mem_dev), GFP_KERNEL);
    if (!mem_devp) {
        result = -ENOMEM;
        goto fail_malloc;
    }
    memset(mem_devp, 0, sizeof(struct mem_dev));
    //为设备分配内存
    for (i = 0; i < MEMDEV_NR_DEVS; i++) {
        mem_devp[i].size = MEMDEV_SIZE;
        mem_devp[i].data = kmalloc(MEMDEV_SIZE, GFP_KERNEL);
        memset(mem_devp[i].data, 0, MEMDEV_SIZE);
    }
    return 0;
fail_malloc:
    unregister_chrdev_region(devno, 1);
    return result;
}

//模块卸载函数
static void memdev_exit(void)
{
    cdev_del(&cdev);
    kfree(mem_devp);
    unregister_chrdev_region(MKDEV(mem_major, 0), 2);
}
MODULE_AUTHOR("Melvin");
MODULE_LICENSE("GPL");
module_init(memdev_init);
moudle_exit(memdev_exit);
