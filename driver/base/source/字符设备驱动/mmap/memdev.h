/*
 * =====================================================================================
 *
 *       Filename:  memdev.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/08/2018 07:04:19 PM
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
#define MEMDEV_MAJOR 0
#define MEMDEV_NR_DEVS 2
#define MEMDEV_SIZE 4096

struct mem_dev{
    char*data;
    unsigned long size;
};
#endif
