/*
 * =====================================================================================
 *
 *       Filename:  signal_alarm.c
 *
 *    Description:  定时程序
 *
 *        Version:  1.0
 *        Created:  03/02/2018 03:05:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
static void sig_alarm(int sig){
    printf("信号 alarm收到=%d\n",sig);
}
static unsigned int my_sleep(unsigned int nsecs){
    struct sigaction new_act,old_act;
    unsigned int unsleept;
    new_act.sa_handler=sig_alarm;
    //没有掩码
    sigemptyset(&new_act.sa_mask);
    new_act.sa_flags=SA_SIGINFO;
    sigaction(SIGALRM,&new_act,&old_act);
    alarm(nsecs);
    //等待信号
    pause();
    //再次设置alarm将会清空之前alarm，并返回未计完的时间
    unsleept=alarm(0);
    sigaction(SIGALRM,&old_act,NULL);
    return unsleept;
}
int main(void){
   unsigned int unsleeped;
   printf("程序第一次运行....\n");
   printf("程序将休眠2秒\n");
   unsleeped=my_sleep(2);
   printf("休眠结束,未休眠时间unsleeped=%d\n\n",unsleeped);
   return 0;
}

