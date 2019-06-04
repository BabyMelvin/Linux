/*
 * =====================================================================================
 *
 *       Filename:  app_mmap.c
 *
 *    Description:  mmap 设备映射
 *
 *        Version:  1.0
 *        Created:  04/08/2018 07:08:18 PM
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
#include <sys/mman.h>

int main(){
    int fd;
    char *start,*buf;
    fd=open("/dev/memdev",O_RDWR);
    buf=(char*)malloc(100);
    memset(buf,0,100);
    start=mmap(NULL,100,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    //读出数据
    strcpy(buf,start);
    sleep(1);
    printf("buf ;l= %s\n",buf);

    //写入数据
    strcpy(start,"buf is not null");
    memset(buf,0,100);
    strcpy(buf,start);
    sleep(1);
    printf("buf 2=%s\n",buf);

    munmap(start,100);
    free(buf);
    close(fd);
    return 0;
}

