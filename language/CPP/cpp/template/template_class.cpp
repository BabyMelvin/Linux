/*
 * =====================================================================================
 *
 *       Filename:  template_class.cpp
 *
 *    Description:  类模板
 *
 *                  template <类型形式参数表>
 *                  类声明
 *
 *                  注意:
 *                      类属参数必须至少在类说明中出现一次
 *
 *        Version:  1.0
 *        Created:  05/09/2018 04:43:39 PM
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

//1.单个语法
template<typename T>
class A{
public:
    A(T t){
        this->t=t;
    }
    T & getT(){
        return t;
    }
protected:
    T t;
};


//2.继承语法:子类从模板类继承时候需让编译器知道父类数据类型
class B:public A<int>
{
public:
    B(int i):A<int>(i)
    {

    }

    void printB()
    {
        cout<<"A:"<<t<<endl;
    }
};

int main(){

    A<int> a(100);
    cout<<"a.getT()="<<a.getT()<<endl;

    B b(10);
    b.printB();
    return 0;
}
