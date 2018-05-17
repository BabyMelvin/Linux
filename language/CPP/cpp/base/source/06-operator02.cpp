/*
 * =====================================================================================
 *
 *       Filename:  06-operator02.cpp
 *
 *    Description:  一元操作
 *
 *        Version:  1.0
 *        Created:  05/17/2018 02:59:56 PM
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
using namespace std;
class Complex
{
private:
    int a;
    int b;
    //TODO 友元和限制无关,前置重载
    friend Complex operator++(Complex&c1);

    //TODO 友元，后置重载
    friend Complex operator++(Complex&c1,int);

public:
    Complex(int a,int b){
        this->a=a;
        this->b=b;
    }
    //TODO 前置++，成员函数实现
    Complex& operator--(){
        this->a--;
        this->b--;
       return *this; 
    }
    //TODO 后置++,成员函数
    Complex operator++(int){
        Complex tmp=*this;
        this->a--;
        this->b--;
        return tmp;
    }

    void printCom(){
        cout<<a<<"+"<<b<<"i"<<endl;
    }
}
//TODO 前置++,全局函数
/**
 * 1.操作符重载，operator
 * 2.函数参数个数
 * 3.分析函数返回值
 */
Complex& operator++(Complex &c1){
   c1.a++;
   c1.b++;
   return c1; 
}

//TODO 后置++，全局函数实现
Complex operator++(Complex& c1,int){
    Complex tmp=c1;
    c1.a++;
    c1.b++;
    return tmp;
}

//TODO 友元和全局类似
int main(){
    //TODO 前置，全局函数调用
    Complex c1;
    ++c1;
    c1.printCom();

    return 0;
}

