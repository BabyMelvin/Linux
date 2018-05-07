/*
 * =====================================================================================
 *
 *       Filename:  friend.cpp
 *
 *    Description:  友元函数
 *
 *        Version:  1.0
 *        Created:  05/07/2018 03:43:27 PM
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
private:
    int i;
    //语句位置和访问描述无关(private中也成立)
    friend void friendFun(A*,int);

public:
    void memberFun(int);
}
void friendFun(A*ptr,int x){
    //通过参数访问私有成员
    ptr->i=x;
}

void A::memberFun(int x){
    i=x;
}

int main(){
    
    return 0;
}

