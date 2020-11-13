/*
 * =====================================================================================
 *
 *       Filename:  signal_action.c
 *
 *    Description:  检查和改变信号动作
 *                  int sigaction(int signum,const struct sigaction*act,struct sigaction*oldact);
 *                  signum:不能是SIGKILL(kill -9)和SIGSTOP(ctr-z)
 *
 *
 *        Version:  1.0
 *        Created:  03/02/2018 01:39:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
/*
 * 信号有2，ctrl-c
 * */
static void do_sig(int num)
{
    printf("收到信号,信号处理");
}

int main(void)
{
    struct sigaction act;
    //1.可以默认处理信号
    //act.sa_handler=SIG_DEF;
    //2.可以忽略信号
    //act.sa_handler=SIG_IGN;
    //3.自己处理信息
    act.sa_handler = do_sig;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGINT, &act, NULL);
    while (1) {
        printf("主程序运行中。。。\n");
        sleep(1);
    }
    return 0;
}
