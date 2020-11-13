/*
 * =====================================================================================
 *
 *       Filename:  fifo_ct.c
 *
 *    Description:  创建管道
 *                  $mkfifo filename 
 *
 *        Version:  1.0
 *        Created:  03/01/2018 05:53:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(void)
{
    int fd;
    fd = mkfifo("my_fifo", 0777);
    
    if (fd < 0) {
        perror("mkfifo");
        if (errno != EEXIST) {
            return -1;
        }
    }

    write(fd, "hello,world\n", sizeof("hello.world\n"));
    close(fd);
    return 0;
}
