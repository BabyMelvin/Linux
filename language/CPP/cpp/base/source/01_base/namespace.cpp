/*
 * =====================================================================================
 *
 *       Filename:  namespace.cpp
 *
 *    Description:  
 *              c语言中：
 *                  C语言中只有一个全局作用域
 *              C++中提出了命名空间概念:
 *                  命名空间将全局作用域分成不同的成分
 *
 *              定义形式:
 *                  namespace name{...};
 *              C++命名空间使用:
 *                  整个命名空间:using namespace name;
 *                  使用命名空间中的变量:using name::variable;
 *        Version:  1.0
 *        Created:  05/07/2018 09:44:05 AM
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
namespace NameSpaceA{
    int a=0;
}
namespace NameSpaceB{
    int a=1;
    namespace NameSpaceC{
        struct Teacher{
            char name[10];
            int age;
        };
    }
}
int main(){
    using namespace NameSpaceA;
    using NameSpaceB::NameSpaceC::Teacher;
    printf("a=%d\n",a);
    printf("a=%d\n",NameSpaceB::a);

    NameSpaceB::NameSpaceC::Teacher t2;
        Teacher t1={"aaa",3};
    printf("t1.name=%s\n",t1.name);
    printf("t1.age=%d\n",t1.age);

    return 0;
}
