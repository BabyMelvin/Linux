## register关键字增强
### 1.register关键字变化
register请求"编译器"将局部变量存储于寄存器中

C语言无法取得register变量地址，c++编译器有自己优化方式，不使用register也可能做优化。

C++中可以取得register变量地址

### 2.
c++编译器发现程序中需要取register变量地址时，register对变量的声明变得无效

## 变量检测增强

* c语言中定义多个同名全局变量合法，在c++中，不允许定义多个同名全局变量
* c语言多个同名全局变量最终会被链接到全局数据区的同一个地址空间上

## c++中所有变量和函数都必须有类型

* `int f()`在c语言中表示返回值为int，接收任意参数的函数;`int f(void)`c中标志返回值为int无参数函数
* `int f()`和`int f(void)`在c++中具有相同的意义，都表示返回值为int无参函数

## 新增Bool类型关键字

## 三目运算符功能加强

* c中三目运算返回值
* c++中三目运算返回变量(可作为左值)

## c/c++中的const

* 理解：const是定义常量---》意味着只读
* 常整型数：`const int a;`和`int const b;`
* 常整型数的指针`const int *c;`，指向内存数据不能被修改，指针本身可以修改。
* 常指针： `int *const b;`,指针变量不能被修改，指向内存空间可以被修改。
* 常整型的常指针:`const int * const e;`，指针和指向内存空间，均不能被修改。

const好处

* 1.指针做函数参数，可以有效提高代码可读性，减少bug;
* 2.清楚的分清参数的输入和输出特性；

```c
//const修改形参时候，利用形参不能修改指针指向的内存空间
int setTeacher_err(const Teacher*p)
```
* c语言中的const变量
    * c语言中const变量是只读变量，有自己的存储空间。
* c++中的const常量
    * 可能分配存储空间，也可能不分配存储空间
    * 当const常量为全局，并且需要在其它文件中使用
    * 当使用&操作符取const常量的地址

### const和define相同之处

C++中的const修饰的，是一个真正的常量，而不是c中变量(只读).const修饰常量编译期间，就已经确定下来了。

```c
#define N 10

int main(){
    const int a =1;
    const int b =2;
    int arrary[a+b]={0};
    int i = 0;
    for (i=0;i<(a+b);i++){
        printf("array[%d]=%d\n",i,array[i]);
    }
    getchar();
    return 0;
}
```

### const和define的区别

* c++中const常量类似于宏定义,`const int c=5;`类似于`#define c 5`
* c++中的const常量与宏定义不同
    * const常量是由编译器处理的，提供类型检查和作用域检查
    * 宏定义由预处理器处理，单纯的文本替换

### 结论

* c语言中的const变量
    * c语言中const变量是只读变量，有自己的存储空间。
* c++中const常量
    * 可能分配存储空间，也可能不分配存储空间
    * 当const常量为全局，并且需要在其他文件使用，会分配存储空间
    * 当使用&操作符，取const常量的地址时，会分配存储空间
    * 当`const int &a=10;`const修饰引用时，也会分配存储空间。




