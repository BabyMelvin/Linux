/*
 * =====================================================================================
 *
 *       Filename:  friend02.cpp
 *
 *    Description: 友元类 
 *
 *        Version:  1.0
 *        Created:  05/07/2018 03:48:35 PM
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
class A{
    //B是A的友元
    friend class B;
public:
    void display(){cout<<x<<endl;}
private:
    int x;
};
class B{

public:
    //TODO B可以访问A的私有
    void set(int i){ object.x=i;};
    void display(){object.display();};
private:
    A object; //B中有个A，A是B的子属性
};

void main(){
    B objectB;
    objectB.set(100);
    objectB.display();
    return 0;
}
