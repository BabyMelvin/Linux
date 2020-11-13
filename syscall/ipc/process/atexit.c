/**
 * 一个进程可以登记多大32个函数，这些函数由exit自动调用
 *
 * atexit来注册函数到终止处理程序
 *      执行顺序和注册顺序相反
 * 运行:
 *      main is done
 *      second exit handler
 *      first exit handler
 * */

#include <stdio.h>
#include <stdlib.h>

static void my_exit(void)
{
    printf("first exit handler\n");
}

static void my_exit2(void)
{
    printf("second exit handler\n");
}

int main(void)
{
    if (atexit(my_exit) != 0) {
        printf("Can't register my_exit\n");
        return -1;
    }

    if (atexit(my_exit2) != 0) {
        printf("Can't register my_exit2\n");
        return -1;
    }

    printf("main is done.\n");
    return 0;
}
