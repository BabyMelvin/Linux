/*
 * =====================================================================================
 *
 *       Filename:  plat_device.c
 *
 *    Description:  platform驱动模型
 *                  首先加载plat_device内核模块。
 *                      模块初始化函数platform_init被调用,
 *                      该函数完成plat_device的定义和注册操作
 *                  再加载pla_buttons内核模块
 *                      buttons_init被调用，该函数完成plat_driver定义和注册操作
 *                  TODO plat_driver注册时内核会在相应的platform总线上去遍历所有匹配
 *                      做匹配操作，匹配函数platform_match(/drivers/base/platform.c)
 *                      匹配方式为平台设备定义的名字和驱动注册的名字是否一致；
 *                          如果匹配成功，说明注册的驱动能驱动该设备，则驱动中
 *                          mini2440_buttons_probe(plat_buttons.c)被调用。该函数
 *                          完成6个按键中断注册，以及采用混杂字符设备驱动的方式注册
 *                          按键驱动。
 *
 *        Version:  1.0
 *        Created:  04/09/2018 10:56:58 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <mach/regs-gpio.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/io.h>

//平台资源的定义

static struct resource s3c_buttons_resource[]={
    [0]={
        .start=S3C24XX_PA_GPIO                          //资源起始物理地址
        .end  =S3C24XX_PA_GPIO + S3C24XX_SZ_GPIO -1,    //资源结束物理地址
        .flags=IORESOURCE_MEM                           //资源类型，MEM,IO,IRQ
    },
    [1]={
        .start  = IRQ_EINT8,   //TODO EINT外部中断
        .end    = IRQ_EINT8,
        .flags  = IORESOURCE_IRQ,
    },
    [2]={
        .start  = IRQ_EINT11,
        .end    = IRQ_EINT11,
        .flags  = IORESOURCE_IRQ, 
    },
    [3]={
        .start  = IRQ_EINT13,
        .end    = IRQ_EINT13,
        .flags  = IORESOURCE_IRQ,
    },
    [4]={
        .start  = IRQ_EINT14,
        .end    = IRQ_EINT14,
        .flags  = IORESOURCE_IRQ,
    },
    [5]={
        .start  = IRQ_EINT15,
        .end    = IRQ_EINT15,
        .flags  = IORESOURCE_IRQ,
    },
    [6]={
        .start  = IRQ_EINT19,
        .end    = IRQ_EINT19,
        .flags  = IORESOURCE_IRQ,
    }
};
static struct platform_device*s3c_buttons;
static int __init platform_init(void){
    //(name,id) 设备id 一般为-1
    s3c_buttons=platform_device_alloc("mini2440-buttons",-1);

    // 设备添加resource
    platform_devices_add_resource(s3c_buttons,&s3c_buttons_resource,7);

    //平台设备的注册(struct platform_device*pdev)
    platform_device_add(s3c_buttons);
}

static void __exit platform_exit(void){
    platform_device_unregister(s3c_buttons);
}
module_init(platform_init);
module_exit(platform_exit);
MODULE_AUTHOR("Melvin");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:mini2440buttons");

