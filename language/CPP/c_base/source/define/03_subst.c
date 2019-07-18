/**
 * 字符串转换
 *      字符串中包含宏参数,把宏参数转换成字符串
 *      C语言中将三个字符串自动连接成一个字符串
 * */

#include <stdio.h>
#define PSQR(x) printf("The square of " #x " is %d.\n", ((x) * (x)))
#define PR(x) printf(" "#x" \n");

int main(void)
{
    int y = 5;

    PSQR(y); // "y" -> #x 转换成字符串
    //printf("The square of " "y" " is %d.\n", ((x) * (x)));    
    PSQR(2 + 4);

    return 0;
}

