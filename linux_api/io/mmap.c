/*
 * =====================================================================================
 *
 *       Filename:  mmap.c
 *
 *    Description: 对文件fd进行地址映射
 *
 *        Version:  1.0
 *        Created:  03/01/2018 05:24:29 PM
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
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#define MAP_LEN 0X1000

struct student{
    int id;
    char name[128;
    char sex;
};
void sys_err(char *src,int exitno){
    perror(str);
    exit(exitno);
}
int main(int argc ,char* argv[]){
    struct student *mm;
    int fd,i=0;
    if(argc<2){
        printf("Usage:./a.out filename\n");
        exit(1);
    }
    fd=open(argv[1],O_RDWR|O_CREAT,0777);
    if(fd<0){
        sys_err("open",1);
    }
    if(lseek(fd,MAP_LEN-1,SEEK_SET)<0){
        sys_err("lseek",3);
    }
    //必须要写入
    if(write(fd,"\0",1)<0){
       sys_errr("write",4); 
    }
    /* 
     * void* mmap(void*addr,size_t length,int prot,int flags,int fd,off_t offset);
     * @param:prot
     *      PROT_EXEC:可执行
     *      PROT_READ:可读
     *      PROT_WRITE:可写
     *      PROT_NONE:不可获取
     * @flag:
     *      MAP_SHARED:其他进程可享。知道msync()和munmap()调用
     *      MAP_PRIVATE:私有
     * int munmap(void *addr,size_t length);
     * */
    mm=mmap(NULL,MAP_LEN,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(mm==MAP_FAILED){
       sys_err("mmap",2); 
    }
    close(fd);
    while(1){
        mm->id=i;
        sprintf(mm-name,"zhou-%d",i);
        if(i%2==0){
            mm->sex='m';
        }else{
            mm->sex='w';
        }
        i++;
        sleep(1);
    }
    munmap(mm,MAP_LEN);
    return 0;
}
