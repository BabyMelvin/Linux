/*
 * =====================================================================================
 *
 *       Filename:  07-friend03.cpp
 *
 *    Description:  vector类重载
 *                  重载流插入和提取运算符
 *
 *        Version:  1.0
 *        Created:  05/17/2018 04:27:55 PM
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
class Vector{
public:
    Vector(int size=1);
    ~Vector();
    int& operator[](int i);
    friend ostream& operator <<(ostream& out,Vector&);
    friend istream& operator >>(istream&,Vector&);
private:
    int *v;
    int len;
};

Vector::Vector(int size){
    if(size<=0||size>100){
        cout<<"the size of"<<size<<"is null!";
        abort();
    }
    v=new int[size];
    len=size;
}
Vector::~Vector(){
    delete[] v;
    len=0;
}
int & Vector::operator[](int i){
    if(i>0&&i<len){
        return v[i];
    }
    cout<<"the subscript"<<i<<"is outside!";
    abort();
}
ostream& operator<<(ostream &output,Vector& ary){
    for(int i=0;i<ary.len;i++){
        output<<any[i]<<" ";
    }
    output<<endl;
    return output;
}
istream& operator>>(istream& input,Vector& ary){
    for(int i=0;i<ary.len;i++){
        input>>ary[i];
    }
    return input;
}

void main(){
    int k;
    cout<<"input the length of vector:\n";
    cin>>k;
    Vector A(k);
    cout<<"input the element of vector :\n";
    cin >>A;
    cout<<"output the elements of vector A:\n";
    cout<<A;

}












