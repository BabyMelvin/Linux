/*
 * =====================================================================================
 *
 *       Filename:  09-inheritance-derive02.cpp
 *
 *    Description:  父类和子类的关系
 *
 *        Version:  1.0
 *        Created:  05/17/2018 06:22:09 PM
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

class  Parent03
{
protected:
    const char* name;
public:
    Parent03()
    {
        name="parent03";
    }
    void print()
    {
        cout<<"Name:"<<name<<endl;
    }
};

class Child03:public Parent03
{
protected:
    int i;
public:
    Child03(int i){
        this->name="child03";
        this->i=i;
    }
};
int main(){
    Child03 child03(1000);
    //分别定义1.父类对象，2.父类指针 3.引用child
    Parent03 parent=child03;
    Parent03* pp=&child03;
    Parent03& rp=child03;

    parent.print();
    pp->print();
    rp.print();
    return 0;
}

