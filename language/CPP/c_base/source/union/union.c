/**
 * 联合体
 *  一个能在同一个存储空间里(但不同时)存储不同类型的数据的结构体
 **/

#include <stdio.h>
#include <stdlib.h>

union hold {
    int digit;
    double bigfl;
    char letter;
};

int main(void)
{
    //TODO fit变量，最大为double,8个字节
    union hold fit;
    fit.digit = 23;  //把23存储在fit中，使用2个字节
    fit.bigfl = 2.0; //清楚23存储2.0,使用8个字节
    fit.letter = 'h'; //清楚2.0，存储'h',使用1个字节

    //TODO C99初始化版本
    union hold valA;
    valA.letter = 'R';
    union hold valB = valA; //把一个联合体初始化另一个联合
    union hold valC = {88}; //初始化联合的digit成员
    union hold valD = {.bigfl = 118.2};

    union hold save[10];
    union hold *pu;

    pu = &fit;

    //TODO 下面是错误的
    float flnum;
    fit.letter = 'A';
    //flnum = 3.02 * fit.bigfl; //TODO 错误的 因为fit是letter字符，而不是double类型 
}


