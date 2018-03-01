/*
 * =====================================================================================
 *
 *       Filename:  ioctl_fb.c
 *
 *    Description:  获取framebuffer信息
 *                  FBIOGET_FSCREENINFO:与framebuffer相关固定信息。
 *                                      图形硬件实际帧缓存空间大小，能都硬件加速等
 *
 *                  FBIOGET_VSCREENINFO:可变信息，framebuffer长度，宽度以及颜色深度等信息
 *
 *        Version:  1.0
 *        Created:  03/01/2018 10:33:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <string.h>

char *fb_addr;
unsigned fb_size;
char *fd_dev="/dev/fb0";
int print_screen(char *buf,int width,int height);

int main(int argc,int argv){
   int screen_fbd=0;
   //固定信息
   struct fb_fix_screeninfo fb_fix; 
   //可变信息
   struct fb_var_screeninfo fb_var;
   short*picture;
   screen_fbd=open(fb_dev,O_RDWR);
   if(screen_fbd<0){
        perror("open");
        printf("打开设备%s失败\n",fb_dev);
        exit(1);
   }

   ioctl(screen_fbd,FBIOGET_FSCREENINFO,&fb_fix);
   //line_length一行的字节数
   printf("fb_fix.line_length=%d,fb_fix.accel=%d\n",fb_line_length,fb_fix.accel);
   
   ioctl(screen_fbd,FBIOGET_VSCREENINFO,&fb_var);
   //xres和yres是真实显示分辨率，行数和列数
   printf("fb_var.xres=%d,fb_var.yres=%d\n",fb_var.xres,fb_var.yres);

   //行数*一行的字节数
   fd_size=fb_var.yres*fb_fix.line_length;

   //fb_addr,或者设备共享内存的地址
   fb_addr=(char*)mmap(NULL,fb_size,PROT_READ|PROT_WRITE,MAP_SHARED,screen_fbd,0);
   
   picture=(char*)malloc(fb_var.yres*fb_fix.line_length);

   //每次赋值一半8bit，实际是16bit.
   memset(picture,0XFF,fb_var.yres*fb_fix.line_length);
   print_screen(picure,fb_var.xres,fb_var.yres); 
   return 0;
}

/* *
 * 字节数=像素*2
 * width和height指的像素
 * */
int print_screen(char*buf,int width,int height){

   //转成short类型，则直接+像素即是偏移，不用乘以2
   short *t_data=(short*)buf; 
   //同上
   short *t_fb_addr=(short*)fb_addr;

   int bytew=width<<1;
   while(--height>=0){
        memcpy(t_fb_addr,t_data,bytew);
        t_fb_addr+=width;
        t_data+=width;
   }
}
































































