/*
 * =====================================================================================
 *
 *       Filename:  class_object04.cpp
 *
 *    Description:  静态成员变量和静态成员函数
 *                  关键字：
 *                      static可以说明一个类的成员，
 *                      静态成员提供了一个同类对象的共享机制
 *                      static类成员时，所有对象，共享这个static成员
 *
 *
 *        Version:  1.0
 *        Created:  05/07/2018 02:59:15 PM
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
class Counter{
    //申明与定义静态数据成员
    static int num;

public:
    static int sMem;//公有静态成员数据
    int mem;//公有数据成员 

    //成员函数访问静态数据成员
    void setNum(int i){num=i;}
    void showNum(){cout<<num<<'t';}

    /* 
     * 静态成员函数关键字static
     * 静态成员函数提供不依赖与类数据结构的共同操作，它没有this指针
     * 在类外调用静态成员函数用"类名::"作限定词，或通过对象调用
     * */
    static void getMem(){
      //TODO 类静态数据成员函数，不能调用普通成员变量mem  
       //cout<<mem<<endl; 
       cout<<sMem<<endl;
    }
};
//申明与定义静态数据成员
int Counter::num=0;
int Counter::sMem=1;
void main(){
    Counter a,b;
    a.showNum();
    b.showNum();
    a.setNum(10);
    a.showNum();
    b.showNum();

    count<<a.sMem<<endl;//访问静态成员方法1
    cout<<Counter::sMem<<endl;//访问静态成员方法2
}
