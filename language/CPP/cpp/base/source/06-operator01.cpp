/*
 * =====================================================================================
 *
 *       Filename:  operator01.cpp
 *
 *    Description:  complex
 *                  语法形式
 *                      类型 类名 :: operator op(参数表)
 *
 *        Version:  1.0
 *        Created:  05/07/2018 04:17:34 PM
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

class Complex{
public:
    int a;
    int b;
    friend Complex operator+(Complex &c1,Complex &c2);
public:
    Complex(int a=0,int b=0){
        this->a=a;
        this->b=b;
    }
    void pritCom(){
        cout<<a<<"+"<<b<<"i"<<endl;
    }
private:
};

Complex operator+(Complex &c1,Complex &c2){
    Complex tmp(c1.a+c2.a,c1.b+c2.b);
    return tmp;
}

void main(){
    Complex c1(1,2),c2(3,4);
    Complex c3=c1+c2;    
}

