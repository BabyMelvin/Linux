/*
 * =====================================================================================
 *
 *       Filename:  pthread.c
 *
 *    Description:  线程创建
 *
 *        Version:  1.0
 *        Created:  03/02/2018 03:37:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static void* pthread_fun(void* arg)
{
    int* p = (int*)arg;
    printf("pthread进程 id=%d\n", getpid());
    printf("pthread 线程id=%x\n", (unsigned int)pthread_self());
    printf("pthread *arg+%d\n", *p);
    while (1)
        ;
}
int main(void)
{
    pthread_t tid;
    int n = 10;
    pthread_create(&tid, NULL, pthread_fun, (void*)&n);
    printf("主进程 id=%d\n", getpid());
    printf("主线程 id=%x\n", (unsigned int)pthread_self());
    printf("子线程 id=%x\n", (unsigned int)tid);
    //避免主线程退出导致子线程也结束
    while (1)
        ;
    return 0;
}
