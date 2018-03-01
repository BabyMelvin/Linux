/*
 * =====================================================================================
 *
 *       Filename:  read_write.c
 *
 *    Description:  文件读写操作
 *                  阻塞读文件
 *
 *        Version:  1.0
 *        Created:  03/01/2018 10:02:21 AM
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
#include <sys/stat.h>

int main(void){
    char buf[1024];
    int len;

    len=read(STDIN_FILENO,buf,sizeof(buf));
    write(STDOUT_FILENO,buf,len);
    return 0;
}

