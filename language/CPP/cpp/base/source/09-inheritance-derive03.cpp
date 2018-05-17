/*
 * =====================================================================================
 *
 *       Filename:  09-inheritance-derive03.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/17/2018 06:28:30 PM
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

class Parent04
{
public:
    Parent04(const char *s)
    {
        cout<<"parent04()"<<" "<<s<<endl;
    }
    ~Parent04()
    {
        cout<<"~Parent04()"<<endl;
    }
};

class Child04:public Parent04
{
public:
    Child04():Parent04("parameters from child!")
    {
        cout<<"child04()"<<endl;
    }
    ~Child04()
    {
        cout<<"~Child04()"<<endl;
    }
};

int run(){
    Child04 child;
}
int main(){
    run();
}

