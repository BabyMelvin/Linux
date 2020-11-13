/*
 * =====================================================================================
 *
 *       Filename:  fcntl.c
 *
 *    Description:  获取和改变文件属性
 *                  int fcntl(int fd,int cmd,...);
 *                  fcntl有5种功能:
 *                      1.复制一个已有的文件描述符(cmd=F_DUPFD或F_DUPEFD_CLOEXEC)
 *                      2.设置或更改文件描述标志(cmd=F_GETFD或F_SETFD)
 *                      3.获取/设置文件状态标志(cmd=F_GETFL或F_SETFL)
 *                      4.获取/设置异步I/O所有权(cmd=F_GETOWN或F_SETOWN)
 *                      5.获取/设置纪录锁(cmd=F_GETLK,F_SETLK或F_SETLKW)
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
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    int fd;
    int flag;
    fd = open("cops", O_RDONLY);
    flag = fcntl(fd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
    close(fd);
    return 0;
}
