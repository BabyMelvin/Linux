/*
 * =====================================================================================
 *
 *       Filename:  class_object02.cpp
 *
 *    Description:  浅拷贝和深拷贝
 *                      1.显示提供copy构造函数
 *                      2.显示操作重载-号操作，不使用编译器提供的浅copy
 *
 *        Version:  1.0
 *        Created:  05/07/2018 02:05:55 PM
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
class Name{
public:
    Name(const char*pName){
       size=strlen(pName);
       pName=(char*)malloc(size+1); 
       strcpy(this->pName,pName);
    }
    Name(const Name& obj){
        //用obj来初始化自己
        pName=(char*)malloc(obj.size+1);
        strcpy(this->pName,pName);
        size=obj.size;
    }
    void operator=(Name &obj3){
        if(pName!=NULL){
            free(pName);
            pName=NULL;
            size=0;
        }
        cout<<"测试有人调用我？"<<endl;
        //用obj来=自己
        pName=(char*)malloc(obj.size+1);
        strcpy(pName,obj.pName);
        size=obj.size;
    }
    ~Name(){
        cout<<"开始析构"<<endl;
        if(pName!=NULL){
            free(pName);
            pName=NULL;
            size=0;
        }
    }
protected:
private:
    char *pName;
    int size;
};

void main(){
    //TODO 对象初始化和对象之间=号操作是两个不同的概念
    Name obj1("obj1..");
    Name obj2=obj1;//obj2创建并初始化
    Name obj3("obj3。。");

    //重载=号操作
    obj2=obj3;//=号操作
}
