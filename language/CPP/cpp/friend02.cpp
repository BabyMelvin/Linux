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

class A{
    
    //B是A的友元
    friend class B;

public:
private:
};
class B{
public:
    void set(int i){ object.x=i;};
    void display(){object.display();};
private:
   A object; 
};

void main(){
    B objectB;
    objectB.set(100);
    objectB.display();
    return 0;
}
