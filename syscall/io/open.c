/*
 * =====================================================================================
 *
 *       Filename:  open.c
 *
 *    Description:  创建多个文件
 *                  最大创建个数的限制(不关闭文件)
 *                  mode中umask限制问题
 *                  
 *                  O_APPEND:写文件追加到结尾
 *                  O_CREAT :不存在就创建
 *                  O_EXCL  :重复创建则报错
 *                  O_TRUNC :文件存在则打开成功后截取为0
 *
 *        Version:  1.0
 *        Created:  03/01/2018 09:41:24 AM
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
#include <unistd.h>
#include <string.h>

int main(int argc,char* argv[]){
    int fd;
    char name[1024];
    int i=0;
    while(true){
        sprintf(name,"file%d",++i);
        umask(0);
        fd=open(name,O_CREAT|O_RDWR|O_EXCL,0777);
        if(fd==-1)
            exit(1);
        else{
            //关闭文件会释放所有的记录锁，进程结束，内核关闭所有打开的文件.
            close(fd);
        }
        printf("第%d 个文件已被创建\n",i);
    }
    return 0;
}

