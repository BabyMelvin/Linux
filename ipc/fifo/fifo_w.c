/*
 * =====================================================================================
 *
 *       Filename:  fifo_w.c
 *
 *    Description:  向管道写数据
 *                  O_NONBLOCK disable:(默认)阻塞直到有相应进程为读而打开改FIFO
 *                  O_NONBLOCK enable: 立刻返回失败，错误码ENXIO
 *
 *        Version:  1.0
 *        Created:  03/01/2018 05:59:43 PM
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
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
void sys_err(char *str,int exitno){
    perror(str);
    exit(exitno);
}
int main(void){
    char buf[1024]="hello world ... \n";
    int len;
    fd=open("my_fifo",O_WRONLY|O_NONBLOCK);
    if(fd<0){
        sys_err("open",1);
    }
    len=write(fd,buf,strlen(buf));
    if(len<0){
        if(errno!=ENXIO){
            sys_err("write");
        }else{
            printf("非阻塞，但是没有读写端没有打开\n");
        }
        close(fd);
    }
    close(fd);
    return 0;
}

