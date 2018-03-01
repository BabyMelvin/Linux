/*
 * =====================================================================================
 *
 *       Filename:  ioctl.c
 *                  int ioctl(int d,int request,...);
 *
 *    Description:  获取终端设备窗口大小 
 *                  read/write:控制in-band数据,I/O操作主体
 *                  ioctl:向设备发控制和配置命令,控制out-of-band数据
 *                        命令传送控制信息 ,数据是辅助信息
 *                  例如：
 *                     串口中波特率、校验位、停止ioctl设置
 *                     A/D转换结果通过read获取，A/D转换精度和频率通过ioctl设置 
 *
 *        Version:  1.0
 *        Created:  03/01/2018 10:17:00 AM
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
#include <unistd.h>
#include <sys/ioctl.h>

int main(void){
    struct winsize size;
    if(isatty(STDOUT_FILENO)==0){
        printf("不是tty终端文件，如果是>0\n");
        exit(1);
    }
    if(ioctl(STDOUT_FILENO,TIOCGWINSZ,&size)<0){
        perror("ioctl:获取窗口大小失败");
        exit(1);
    }
    printf("行宽=%d,列高=%d,总大小=%d\n",size.ws_row,size.ws_col,size);
    return 0;
}

