/*
 * =====================================================================================
 *
 *       Filename:  signal.c
 *
 *    Description:  信号处理
 *
 *        Version:  1.0
 *        Created:  03/02/2018 01:20:03 PM
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
#include <errno.h>
#include <signal.h>
static void sig_handler(int signal){
    printf("接收到中断信号\n");
    printf("信号=%d\n",signal);
}
int main(void){
    int i;
    //处理中断信息,相当于信号拦截,中断后，主程序任然继续运行
    if(signal(SIGINT,sig_handler)==SIG_ERR){
        perror("signal");
        exit(-1);
    }
    for(i=0;;i++){
        printf("程序运行中--%d\n",i);
        sleep(2);
    }
    return 0;
}


