/*
 * =====================================================================================
 *
 *       Filename:  class_object03.cpp
 *
 *    Description:  对象初始化列表
 *                  格式:
 *                      Constructor::constructor():m1(v1),m2(v2),m3(v3){
 *                          //some other assignment operation
 *                      }
 *
 *        Version:  1.0
 *        Created:  05/07/2018 02:35:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
using namespace std;

class ABC{
public:
    ABC(int a,int b,int c){
        this->a=a;
        this->b=b;
        this->c=c;
        printf("ABC constructor...");
    }
    ~ABC(){
        printf("~ABC()...");
        printf("a:%d b:%d c:%d\n",a,b,c);
    }
protected:
private:
    int a;
    int b;
    int c;
};
class MyD{
public:
    MyD():abc1(1,2,3),abc2(4,5,6),m(100)
    {
        cout<<"MyD()"<<endl;
    }
    ~MyD()
    {
        cout<<"~MyD()"<<endl;
    }
protected:
private:
    ABC abc1;//c++编译器不知道如何构造abc1
    ABC abc2;
    const int m;
};
int main(){
    MyD myD;
    return 0;
}


