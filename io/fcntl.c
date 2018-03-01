/*
 * =====================================================================================
 *
 *       Filename:  fcntl.c
 *
 *    Description:  获取和改变文件属性
 *
 *        Version:  1.0
 *        Created:  03/01/2018 11:55:52 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

int main(){
    int fd;
    int flag;
    fd=open("cops",O_RDONLY);
    flag=fcntl(fd,GETFL);
    flg|=O_NONBLOCK;
    fcntl(fd,SETFL,flag);
    close(fd);
    return 0;
}
