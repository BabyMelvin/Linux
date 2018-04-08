/*
 * =====================================================================================
 *
 *       Filename:  memdev.h
 *
 *    Description:字符设备驱动头文件
 *
 *        Version:  1.0
 *        Created:  04/08/2018 02:14:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _MEMDEV_H_
#define _MEMDEV_H_

#define MEMDEV_MAJOR    251 //预设的mem主设备号

#define MEMDEV_NR_DEVS  2 //设备数

#define MEMDEV_SIZE     4096

//mem设备描述结构体
struct mem_dev{
    char *data;
    unsigned long size;
    struct semaphore sem;//定义信号量
    wait_queue_head_t inq;//等待队列，实现进程阻塞
};

//ioctl命令定义:命令编码几个位段,类型(幻数)，序号，传送方向，参数大小
//定义幻数
#define MEMDEV_IOC_MAGIC 'k'

//定义命令
#define MEMDEV_IOCPRINT     _IO(MEMDEV_IOC_MAGIC,1)
#define MEMDEV_IOCGETDATA   _IOR(MEMDEV_IOC_MAGIC,2,int)
#define MEMDEV_IOCSETDATA   _IOW(MEMDEV_IOC_MAGIC,3,int)

#define MEMDEV_IOC_MAXNR 3
#endif

