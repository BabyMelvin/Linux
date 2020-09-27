/**
 * 获取已终止线程的退出码
 * */

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* thr_func1(void* arg)
{
    printf("thread 1 returing \n");
    return ((void*)1);
}

void* thr_func2(void* arg)
{
    printf("thread 2 exiting\n");
    pthread_exit((void*)2);
}

int main(void)
{
    int ret;
    pthread_t tid1, tid2;

    void* tret;

    ret = pthread_create(&tid1, NULL, thr_func1, NULL);
    if (ret != 0) {
        printf("can't create thread 1:%s\n", strerror(ret));
        return -1;
    }

    ret = pthread_create(&tid2, NULL, thr_func2, NULL);
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
