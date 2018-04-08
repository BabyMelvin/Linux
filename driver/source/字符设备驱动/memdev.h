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
};
#endif

