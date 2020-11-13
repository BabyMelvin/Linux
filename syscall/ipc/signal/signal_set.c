/*
 * =====================================================================================
 *
 *       Filename:  signal_set.c
 *
 *    Description:  信号集操作
 *                  //检查或改变阻塞了的信号(默认都阻塞?)
 *                  int sigprocmask(int how,const sigset_t*set,sigset_t*oldset);
 *                      how:
 *                          SIG_BLOCK:参数和当前集合组成所有阻塞信号集
 *                          SIG_UNBLOCK:将参数信号解阻塞
 *                          SIG_SETMASK:参数即是当前的阻塞集合
 *                  //阻塞期间接收到阻塞信号合集
 *                  int sigpending(sigset_t * set);
 *
 *                  #命令kill
 *                  $kill -l 
 *                      列出所有的信号
 *                  #命令trap
 *                  #1.自己处理(可以多个信号)
 *                  $trap "command;command;exit" 1 2
 *                  #2.忽略信号
 *                  $trap "" 2
 *                  #3.恢复信号
 *                  $trap 1 2
 *        Version:  1.0
 *        Created:  03/02/2018 01:59:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
static void print_signal(char* message, const sigset_t* set)
{
    int i;
    printf("\n\n++++++++++++++ %s +++++++++++++++++++++++\n", message);
    for (i = 1; i < 65; i++) {
        if (sigismember(set, i) == 1) {
            printf("OO在阻塞集合中的信号:%d\n", i);
        } else {
            printf("不在阻塞集合中的信号:%d\n", i);
        }
    }
}
int main(void)
{
    sigset_t s, p;
    sigemptyset(&s);
    //将所有信号添加到阻塞集合变量中
    sigfillset(&s);
    print_signal("sigfill", &s);

    //清空，没有任何阻塞，sigismember=0
    sigemptyset(&s);
    sigaddset(&s, SIGINT);
    //检查和更新信号块,阻塞信号加上SIGINT
    sigprocmask(SIG_BLOCK, &s, NULL);

    sigdelset(&s, SIGINT);
    sigprocmask(SIG_BLOCK, &s, NULL);
    print_signal("sigprocmask SIGINIT", &s);

    //检测哪些信号未阻塞
    while (1) {
        sigpending(&p);
        print_signal("sigpending", p);
    }
    return 0;
}
