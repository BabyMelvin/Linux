/*
 * =====================================================================================
 *
 *       Filename:  09-inheritance-derive01.cpp
 *
 *    Description:  多继承
 *                  访问权限的改变
 *
 *        Version:  1.0
 *        Created:  05/17/2018 06:05:54 PM
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

class A{
private:
    int a;
protected:
    int b;
public:
    int c;

    A()
    {
        a=0;
        b=0;
        c=0;
    };
    void set(int a ,int b,int c){
        this->a=a;
        this->b=b;
        this->c=c;
    }
};
class B:public A
{
public:
    void print()
    {
        //TODO cout<<"a="<<a; err
        cout<<"b="<<b;
        cout<<"c="<<c;
    }
};
class C:protected A
{
public:
    void print()
    {
        //TODO cout<<"a="<<a; //err
        cout<<"b="<<b;
        cout<<"c="<<c;
    }
};
class D:private A
{
public:
    void print()
    {
        //TODO cout<<"a="<<a; //err
        cout<<"b="<<b;
        cout<<"c="<<c;
    }
}

int main(){
    A aa;
    B bb;
    C cc;
    D dd;
    aa.c=100;
    bb.c=100;
    //TODO cc.c=100; err
    //dd.c=100; err

    aa.set(1,2,3);
    bb.set(1,2,3);
    //TODO cc.set(1,2,3);
    //TODO dd.set(23,4);

    bb.print();
    cc.print();
    dd.print();
    return 0;
}
