/*
 * =====================================================================================
 *
 *       Filename:  fifo_r.c
 *
 *    Description:  读FIFO管道
 *                  O_NONBLOCK disable:阻塞知道有写操作
 *                  O_NONBLOCK enabld :立刻返回成功
 *
 *        Version:  1.0
 *        Created:  03/01/2018 06:17:22 PM
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
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void sys_err(char* str, int exitno)
{
    perror(str);
    exit(exitno);
}

int main(int argc, char* argv[])
{
    int fd, len;
    char buf[1024];

    if ((fd = open("my_fifo", O_RDONLY | O_NONBLOCK)) < 0) {
        sys_err("open", 1);
    }
    len = read(fd, buf, sizeof(buf));
    if (len <= 0) {
        return 1;
    }

    write(STDOUT_FILENO, buf, len);
    close(fd);
    return 0;
}
