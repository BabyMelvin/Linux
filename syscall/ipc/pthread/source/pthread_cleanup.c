/**
 * 线程清零处理程序
 * */

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cleanup(void* arg)
{
    printf("cleanup:%s\n", (char*)arg);
}

/*
 * pthread_cleanup_push 和 pthread_cleanup_pop调用匹配起来，否则编译不过
 * */
void* thr_func1(void* arg)
{
    printf("thread 1 start \n");
    pthread_cleanup_push(cleanup, "thread 1 first handler");
    pthread_cleanup_push(cleanup, "thread 1 second handler");
    printf("thread 1 push complete\n");
    if (arg)
        return ((void*)1);

    // 参数为0，则不执行清理函数
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    return ((void*)1);
}

void* thr_func2(void* arg)
{
    printf("thread 2 start \n");
    pthread_cleanup_push(cleanup, "thread 2 first handler");
    pthread_cleanup_push(cleanup, "thread 2 second handler");
    printf("thread 2 push complete\n");
    if (arg)
        pthread_exit((void*)2);

    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);

    pthread_exit((void*)2);
}

int main(void)
{
    int ret;
    pthread_t tid1, tid2;

    void* tret;

    ret = pthread_create(&tid1, NULL, thr_func1, (void*)1);
    if (ret != 0) {
        printf("can't create thread 1:%s\n", strerror(ret));
        return -1;
    }

    ret = pthread_create(&tid2, NULL, thr_func2, (void*)1);
    if (ret != 0) {
        printf("can't create thread 1:%s\n", strerror(ret));
        return -1;
    }

    ret = pthread_join(tid1, &tret);
    if (ret != 0) {
        printf("can't join with thread 1:%s\n", strerror(ret));
        return -1;
    }

    printf("thread 1 exit code %d\n", (int)tret);

    ret = pthread_join(tid2, &tret);
    if (ret != 0) {
        printf("can't join with thread 2:%s\n", strerror(ret));
        return -1;
    }

    printf("thread 2 exit code %d\n", (int)tret);
    exit(0);
}
