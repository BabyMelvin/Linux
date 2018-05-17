/*
 * =====================================================================================
 *
 *       Filename:  06-operator03.cpp
 *
 *    Description:  用友元函数重载 << >>
 *
 *        Version:  1.0
 *        Created:  05/17/2018 03:56:52 PM
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
class Complex{
private:
    int a;
    int b;
    friend ostream& operator<<(ostream& out,Complex &c1);
public:
    Complex(int a,int b){
        this->a=a;
        this->b=b;
    }
    ~Complex(){

    }
}

//TODO 友元函数,调用私有的，只能使用友元函数
ostream& operator<<(ostream& out,Complex &c1){
    out<<c1.a<<"+"<<c1.b<<"i"<<endl;
    return out;
}
int main(){
    Complex c(1,2);
    cout<<c1<<endl;
    return 0;
}

