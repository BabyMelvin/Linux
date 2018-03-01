/*
 * =====================================================================================
 *
 *       Filename:  fork.c
 *
 *    Description:  创建子进程
 *
 *        Version:  1.0
 *        Created:  03/01/2018 06:31:43 PM
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
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main(void){
    pid_t pid;
    int n=10;
    //一次调用两次返回
    pid=fork();
    if(pid>0){
        //父进程中
        while(1){
            printf("i am in parent n=%d,my pid=%d my-parent pid=%d\n",n++,getpid(),getppid());
            sleep(1);
        }
    }else if(pid==0){
        //子进程中
        printf("i am in child,n=%d,my pid=%d, my parent pid=%d\n",n++,getpid(),getppid());
        execl("/usr/bin/firefox","firefox","wwww.baidu.com",NULL);
        sleep(10);
    }else{
        perror("fork");
        exit(1);
    }
    return 0;
}


