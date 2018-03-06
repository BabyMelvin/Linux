/*
 * =====================================================================================
 *
 *       Filename:  exe.c
 *
 *    Description:  执行其他程序
 *
 *        Version:  1.0
 *        Created:  03/01/2018 05:17:19 PM
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
#include <stdio.h>

int main(void){
    char *argv0[]={"ls","-l",NULL};
    printf("开始：hello world\n");
    //执行 /bin/ls
    execl("/bin/ls","ls","-l",NULL);

    execlp("ls","ls","-l",NULL);

    execv("/bin/ls",argv0);

    execvp("ls",argv0);
    printf("接收: hello world\n");
    return 0;
}

