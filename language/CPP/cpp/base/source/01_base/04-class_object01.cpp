/*
 * =====================================================================================
 *
 *       Filename:  class_object.cpp
 *
 *    Description:  面向对象三大概念
 *                      封装、继承、多态
 *                  struct和class关键字区别
 *                      在struct定义类时，成员默认为public
 *                      在class定义类时，成员默认为private
 *
 *        Version:  1.0
 *        Created:  05/07/2018 11:17:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;
class Test{
    public:
        //有3种构造函数
        //1.无参数构造函数
        Test(){

        }
        //2.带参数构造函数
        Test(int a,int b){
            this->a=a;
            this->b=b;
        }
        //3.赋值构造函数
        Test(const Test&obj){
            cout<<"构造函数，通过另一个对象obj，来初始化"<<endl;
            b=obj.b+10;
        }
        int getM()const{return a;}
        void setM(int val){a=val;}

        int getN()const {return b;}
        void setN(int val){b=val;}

        ~Test(){
            cout<<"析构函数，自动被调用"<<endl;
        }
protected:
private:
        int a;
        int b;
};

int main(){
    int rv=0;
    //TODO 无参
    Test t1;//无参数构造函数调用方法

    //TODO 有参数3中调用方式
    Test t2(100.200);//1.括号法
    Test t3=(300,400);//2.等号法
    Test t4=Test(400,500);

    //TODO 赋值构造函数
    //1.浅拷贝
    Test t5;
    Test t6=t5;//定义变量并初始化，或者t6=t5,编译器提供浅copy
    //2.
    Test t7;
    Test t8(t7);//定义并初始化
    //t8=t7;//用t7等号给t8，编译器给我们提供的浅copy TODO 初始化和等号操作 两个不同概念
    //TODO 匿名对象去留
    //1.返回的匿名对象给另一个同类型的对象，那么匿名对象会被析构
    //2.若返回匿名对象，来初始化另一个同类型对象，匿名对象会直接转成新对象。


    cout<<t1.getM()<<" "<<t1.getN()<<endl;
    cout<<t2.getM()<<" "<<t2.getN()<<endl;

    //定义对象数组时，没有机会进行显示初始化
    //Test arr_2[3]={Test(1,3),Test(),Test()};
    return 0;
}
