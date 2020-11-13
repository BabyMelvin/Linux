/*
 * =====================================================================================
 *
 *       Filename:  pwd.c
 *
 *    Description:  pwd
 *
 *        Version:  1.0
 *        Created:  03/01/2018 11:25:55 AM
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

#define SIZE 4096
int main(void)
{
    char buf[SIZE];
    chdir("/");
    printf("当前工作目录:%s\n", getcwd(buf, sizeof(buf)));

    return 0;
}
