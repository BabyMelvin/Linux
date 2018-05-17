## 1.引用概念

* 可看做一个**已定义变量**的别名
* 引用的语法:`Type & name=var`
* 引用做函数参数，声明时不进行初始化。
* 意义:一定场合替代指针

```c
void main(){
    //c编译器分配4个字节内存--a内存空间别名
    int a=10;
    //b就是a的别名。。
    int &b=a;

    {
        int*p=&a;
        *p=12;
        printf("a %d\n",a);
    }
    b=14;
    printf("a:%d b:%d",a,b);
}
```

## 2.引用做函数参数

* 普通引用时,必须用它的变量进行初始化
* 引用作为函数参数声明时不进行初始化

```cpp
struct Teacher{
    char name[64];
    int age;
};
void printfT(Teacher*pT){
    cout<<pT->age<<endl;
}
//pT是t1变量别名，相当于修改了t1
void printfT2(Teacher &pT){
   //cout<<pT.age<<endl;
   pT.age=33;
}
//pT和t1是两个不同的变量
void printfT3(Teacher pT){
    cout<<pT.age<<endl;
    pT.age=45;//只会修改pT变量，不会修改t1变量
}
```
## 3.引用的本质

* 1.引用在c++中的内部实现是一个常指针。`Type& name <--->Type *const name`
* 2.C++使用常指针内部实现，占用空间大小与指针相同。
* 3.仔细对比间接赋值成立三个条件
    * 1.定义两个变量
    * 2.建立关联 实参取地址传给形参
    * 3.`*p`形参去间接的修改实参的值

## 4.函数返回值是引用(引用当左值)

* 当函数返回值为引用时:
    * 若返回栈变量
        * 不能成为其他引用的初始值
        * 不能作为左值使用
    * 若返回静态变量或全局变量
        * 可以成为其他引用的初始值
        * 即可作为右值使用，也可作为左值使用。

