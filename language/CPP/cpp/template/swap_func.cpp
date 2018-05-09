/*
 * =====================================================================================
 *
 *       Filename:  swap.cpp
 *
 *    Description: template <类型形式参数表>
 *                 类型 函数名(形式参数表){
 *                      语句序列
 *                 }
 *
 *                 类型参数为： typename T1,..,typename Tn或 class T1,class T2
 *
 *                 函数模板
 *                      模板说明和函数定义组成
 *                  模板参数说明的属性参数必须在函数定义中至少出现一次；
 *                  函数参数表中可以使用类属类型参数，也可使用一般类型参数
 *        Version:  1.0
 *        Created:  05/09/2018 04:07:48 PM
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
#include <string.h>
using namespace std;

template <typename T>
void mySwap(T &a, T &b){
    T t;
    t=a;
    a=b;
    b=t;
}

//TODO T:相对于函数内容
template <typename T>
T maxT(T a,T b){
    return a>b?a:b;
}

/* 
 * typename & class
 * */
template<typename T1,typename T2>
void sortArray(T1 *a,T2 num){
    T1 tmp;
    int i,j;
    for(i=0;i<num;i++){
        for(j=i+1;j<num;j++){
            if(a[i]<a[j]){
                tmp=a[i];
                a[i]=a[j];
                a[j]=tmp;
            }
        }
    }
}

template <class T>
void printArray(T *a,int num){
    int i=0;
    for(i=0;i<num;i++){
        cout<<a[i]<<" ";
    }
}


/*
 * 函数模板遇上函数重载
 *      函数模板不允许自动类型转化，普通函数能够进行自动类型转换
 * 1.函数模板可以被像普通函数一样被重载
 * 2.C++编译器优先考虑普通函数
 * 3.如果函数模板可以产生一个更好的匹配，那么选择模板
 * 4.可以通过空模板参数列表语法限定编译器只通过模板匹配
 */
int maxC(int a,int b){
    cout<<"int maxC(int a,int b)"endl;
    return a>b?a:b;
}

template<typename T>
T maxC(T a,T b){
    return a>b?a:b;
}

template<typename T>
T maxC(T a,T b,T c){
    return maxC(maxC(a,b),c);
}
int main(){
    int x=1;
    int y=2;
    //TODO 1.显示调用
    //mySwap<int>(x,y);

    //TODO 2.隐式调用
    mySwap(x,y);
    cout<<"1,2"<<" x="<<x<<" y="<<y<<endl;

    cout<<"max(1,3)="<<maxT(1,3)<<endl;
    float x1=1.2;
    float y1=2.1; 
    mySwap(x1,y1);
    cout<<"1.2,2.1"<<" x1="<<x1<<" y1="<<y1<<endl;

    //sort arry
    int num=0;
    char a[]="ljflawkjlakjfdsjtjljt";
    num=strlen(a);

    cout<<"排序之前"<<endl;
    printArray<char>(a,num);

    cout<<endl<<"排序之后:";
    sortArray(a,num); 
    printArray<char>(a,num);

    //maxC
    int a=1;
    int b=2;
    cout << maxC(a,b)<<endl;//d都符合使用普通
    cout << maxC<>(a,b)<<endl;//显示使用模板
    cout <<maxC(5.0,6.0,7.0);//重载
    cout <<maxC('a',100);//调用普通函数，隐式转化
    return 0; 
}

