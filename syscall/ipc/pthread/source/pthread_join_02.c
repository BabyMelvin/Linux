/**
 * 线程栈是独立的，使用全局变量和malloc分配使用线程的共享
 * 
 * 可连接创建:有些线程不是默认可连接，显示创建爱可连接
 * */

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct foo {
    int a;
    int b;
    int c;
    int d;
};

void print_foo(const char* s, const struct foo* fp)
{
    printf(s);
    printf(" structure at 0x%x\n", (unsigned)fp);
    printf(" foo.a = %d\n", fp->a);
    printf(" foo.b = %d\n", fp->b);
    printf(" foo.c = %d\n", fp->c);
    printf(" foo.d = %d\n", fp->d);
}

void* thr_func1(void* arg)
{
    struct foo foo = { 1, 2, 3, 4 };
    print_foo("thread 1:\n", &foo);
    pthread_exit((void*)&foo);
}

void* thr_func2(void* arg)
{
    printf("thread 2: ID is %ld\n", pthread_self());
    pthread_exit((void*)0);
}

int main(void)
{
    int ret;
    pthread_t tid1, tid2;
    struct foo* fp;
    void* tret;

    // initialize and set thread detached attribute
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    ret
        = pthread_create(&tid1, &attr, thr_func1, NULL);
    if (ret != 0) {
        printf("can't create thread 1:%s\n", strerror(ret));
        return -1;
    }

    pthread_attr_destroy(&attr);

    ret = pthread_join(tid1, (void*)&fp);
    if (ret != 0) {
        printf("can't join with thread 1:%s\n", strerror(ret));
        return -1;
    }
    sleep(1);

    printf("parent starting second thread\n");
    ret = pthread_create(&tid2, NULL, thr_func2, NULL);
    if (ret != 0) {
        printf("can't create thread 1:%s\n", strerror(ret));
        return -1;
    }
    sleep(1);

    print_foo("parent:\n", fp);
    pthread_exit(NULL);
    exit(0);
}
