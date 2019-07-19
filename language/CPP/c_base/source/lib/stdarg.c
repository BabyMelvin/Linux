/**
 * 可变参数，按如下步骤进行：
 * 
 * 1.提供一个使用省略号的函数原型 
 * 2.在函数定义中创建一个va_list类型的变量:va_list ap;
 * 3.用宏把该变量初始化为一个参数列表;va_start(ap, lim); lim为省略号前一个参数
 * 4.用宏访问参数列表;apcopy作为副本，va_copy(apcopy, ap); va_arg(ap, double);
 * 5.用宏完成清理工作;va_end(ap);
 * 
 * 注意:至少得有一个形参和一个省略号，省略号要在最后
 * */
#include <stdio.h>
#include <stdarg.h>

double sum(int, ...);
int main(void)
{
    double s,t;
    s = sum(3,1.1,2.5,13.3);
    t = sum(6,1.1,2.1,13.1,4.1,5.1,6.1);
    printf("return value for"
            "sum(3,1.1,2.5,13.3): %g\n", s);
    printf("return value for"
           "sum(6,1.1,2.1,13.1,4.1,5.1,6.1):%g\n", t);

    return 0;
}

double sum(int limit, ...)
{
    va_list ap;
    double tot = 0;
    int i;
    va_start(ap, limit);

    for (i = 0; i < limit; i++)
        tot += va_arg(ap, double);

    va_end(ap);

    return tot;
}
