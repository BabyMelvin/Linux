/**
 * 预定义宏
 *  __DATE__  预处理的日期(Nov 23 2013)
 *  __FILE__  表示当前源代码文件名的字符串字而量
 *  __STDC__  设置1时，事项遵循C标准
 *  __STDC_HOSTED__ 本机环境设置为1，否则设置为0
 *  __STDC_VERSION__ 支持C99标准，设置为199901L；支持c11标准:设置为20112L
 *  __TIME__ 翻译代码的时间，格式为"hh:mm:ss"
 *  
 *  预定义标识符 __func__只有函数作用域，而宏具有文件作用域
 *  GCC 必须设置-std=c99或-std=c11
 * */

#include <stdio.h>
void why_me();

//TODO 强制使用c11,编译器只支持旧标准，则会编译失败
#if __STDC_VERSION__ != 201112L
#error Not C11
#endif

int main(void)
{
    printf("The file is %s.\n", __FILE__);
    printf("The date is %s.\n", __DATE__);
    printf("The time is %s.\n", __TIME__);
    printf("The version is %ld.\n", __STDC_VERSION__);
    printf("This is line %d.\n", __LINE__);
    printf("This function is %s\n", __func__);
    why_me();

    return 0;
}

void why_me()
{
#line 1000 //把当前行号重置为1000
    printf("The function is %s.\n", __func__);
    printf("This is line %d.\n", __LINE__);
#line 10 "cool.c" //把行号重置为10,把文件名重置为cool.c
    printf("The function is %s.\n", __func__);
    printf("This is line %d.\n", __LINE__);
}


