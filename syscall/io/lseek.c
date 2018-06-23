/*
 * =====================================================================================
 *
 *       Filename:  lseek.c
 *
 *    Description:  扩大文件
 *                  1.文件当前偏移应当是一个非负值．
 *                      普通文件偏移量必须是非负值
 *                      某些设备文件允许是负值
 *                    注意：lseek可能偏移负值，判断应该看是否是-1.
 *                  2.lseek仅将当前偏移量纪录在内核中，不会进行I/O操作，该偏移量用于下一个读写操作．
 *                  3.lseek大于文件长度：
 *                      下一次写将加长该文件，在文件中构成一个空洞．位于文件中没写过字节都被读为０.
 *                      文件空洞不需要分配磁盘块．
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
    int fd=open("file.hole",O_RDWR|O_CREAT,0x777);
    if(fd<0){
        perror("open");
        exit(-1);
    }
    if(write(fd,"1234567890",10)!=10)
        perror("write");
    /* offset_now=10 */

    //扩大一个文件一定要又一次写操作
    //SEEK_CUR | SEEK_END
    if(lseek(fd,16384,SEEK_SET)==-1){
        printf("cannot seek\n");
    }else{
        printf("seek OK\n");
    }
    /* offset_now=16384*/

    if(write(fd,"abcdef",6)!=6)
        perror("write");
    /* offset_now=16390*/

    close(fd);
    exit(0);
}
