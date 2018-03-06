/*
 * =====================================================================================
 *
 *       Filename:  cp.c
 *
 *    Description:  实现复制操作
 *
 *        Version:  1.0
 *        Created:  03/01/2018 11:28:43 AM
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
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#define SIZE 8192
int main(int argc,char*argv[]){
    char buf[SIZE];
    int fd_src,fd_dest,len;
    if(argc<3){
        printf("Usage:cp src dest\n");
        exit(1);
    }
    fd_src=open(argv[1],O_RDONLY);
    fd_dest=open(argv[2],O_CREAT|O_WRONLY|O_TRUNC);
    while((len=read(fd_src,buf,sizeof(buf)))>0){
        write(fd_dest,buf,len);
    }
    close(fd_src);
    close(fd_dest);
    return 0;
}
