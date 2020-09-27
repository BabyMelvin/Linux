/*
 * =====================================================================================
 *
 *       Filename:  09-inheritance-derive04.cpp
 *
 *    Description:  继承与组合混搭
 *                  先父类，后成员变量，最后构造自己
 *
 *        Version:  1.0
 *        Created:  05/17/2018 06:40:25 PM
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

class Object
{
public:
    Object(const char*s)
    {
        cout<<"Object()"<<" "<<s<<endl;
    }
    ~object(){
        cout<<"~Object()"<<endl;
    }
};

class Parent:public Object
{
public:
    Parent(const char*s):Object(s)
    {
        cout<<"Parent()"<<" "<<s<<endl;
    }
    ~Parent()
    {
        cout<<"~Parent()"<<endl;
    }
};
class Child:public Parent
{
protected:
    Obejct o1;
    Obejct o2;
public:
    Child():o2("o2"),o1("o1"),Parent("parameter from child!")
    {
        cout<<"Child()"<<endl;
    }
    ~Child()
    {
        cout<<"~Child()"<<endl;
    }
};
void run(){
    Child();
}

int main(){
    cout<<"main()"<<endl;
    run();
    return 0;
}
