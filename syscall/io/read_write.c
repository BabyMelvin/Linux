/*
 * =====================================================================================
 *
 *       Filename:  read_write.c
 *
 *    Description:  文件读写操作
 *                  阻塞读文件
 *                  1.read:返回读到字节数，文件尾返回0;出错返回-1
 *                      返回小于要求读到字节数：
 *                         普通文件：读到要求字节已达到文件尾.返回剩余字节数，再次调用返回0
 *                         终端文件:通常一次一行
 *                         网络读 :网络缓冲机制，可能造成返回小于要求的字节数．
 *                          管道或FIFO: 剩余不够要求，返回剩余.
 *                   2.write:通常返回值于参数nbytes值相同,否则出错.
 *                     出错可能：
 *                      磁盘满了
 *                      超过给定进程的文件长度限制
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

