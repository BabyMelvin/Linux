/*
 * =====================================================================================
 *
 *       Filename:  lseek.c
 *
 *    Description:  扩大文件
 *
 *        Version:  1.0
 *        Created:  03/01/2018 11:18:55 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int main(void){
    int fd=open("abc",O_RDWR);
    if(fd<0){
        perror("open");
        exit(-1);
    }
    //扩大一个文件一定要又一次写操作
    lseek(fd,0x1000,SEEK_SET);
    write(fd,"a",1);
    close(fd);
}
