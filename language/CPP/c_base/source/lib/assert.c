/**
 * assert.h支持断言辅助调试程序
 * assert()宏
 *  结果真则stderr写入一条粗偶无信息，并调用abort()终止函数
 * */


#include <stdio.h>
#include <math.h>

//如果在assert.h之前定义这个宏，相当于关闭所有判断
//#define NDEBUG
#include <assert.h>

int main(void)
{
    double x, y, z;
    puts("Enter a pair of numbers(0 0 to quit):");
    while (scanf("%lf %lf",&x, &y) == 2
            && (x != 0 || y !=0)) {
        z = x*x - y*y;
        assert(z >= 0);
        printf("answer is %f\n", sqrt(z));
        puts("Next pair of numbers:");
    }
    puts("Done");

    return 0;
}
