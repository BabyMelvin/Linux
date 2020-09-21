# 验证是否为常量表达式
常量表达式，指多个常量组成的表达式。匿名枚举，switch-case，数组的长度必须是一个和常量表达式：

```cpp
// 1)
int url[10]; //正确
int url[6 + 4]; // 正确

int lenght = 6;
int url[lenght]; // 错误，length是变量
```

常量表达式是在编译阶段，节约运行时间。constexpr关键字指定常量表达式，可以修饰变量、函数以及类构造函数.

* C++11 标准中，定义变量时可以用 constexpr 修饰，从而使该变量获得在编译阶段即可计算出结果的能力。
* constexpr还可以用于修饰函数的返回值，这样的函数又称为“常量表达式函数”。一个函数要想成为常量表达式函数，必须满足如下 4 个条件。
    * 1) 整个函数的函数体中，除了可以包含 using 指令、typedef 语句以及 static_assert 断言外，只能包含一条 return 返回语句。
    * 2) 该函数必须有返回值，即函数的返回值类型不能是 void。
    * 3) 函数在使用之前，必须有对应的定义语句。我们知道，函数的使用分为“声明”和“定义”两部分，普通的函数调用只需要提前写好该函数的声明部分即可（函数的定义部分可以放在调用位置之后甚至其它文件中），但**常量表达式函数在使用前**，**必须要有该函数的定义**。
    * 4) return 返回的表达式必须是常量表达式，举个例子：

```cpp
#include <iostream>
using namespace std;

// 普通函数的声明
int nonconst_dis(int x);
// 常量表达式函数的声明
constexpr int display(int x);

// 常量表达式函数的定义
constexpr int display(int x) {
    // 只能添加using typedef或static_assert断言
    return 1 + 2 + x;
}

int main()
{
    constexpr int num = 1 + 2 + 3;
    // 去掉constexpr 报错num不可作为常量
    int url[num] = {1, 2, 3, 4, 5, 6};
    cout << url[1] << endl;

    // 调用常量表达式函数
    int a[display(3)] = {1, 2, 3, 4};
    cout << a[2] << endl;

    // 调用普通函数
    cout << noconst_dis(3) << endl;
    return 0;
}

// 普通函数定义
int noconst_dis(int x) {
    return 1 + 2 + x;
}
```
普通函数在调用时，只需要保证调用位置之前有相应的声明即可；而常量表达式函数则不同，调用位置之前必须要有该函数的定义，否则会导致程序编译失败。

```cpp
// 无法通过编译
#include <iostream>

using namespace std;

int num = 3; // 运行阶段确定
constexpr int display(int x) {
    return num + x;
}

int main() {
    // 调用常量表达式函数
    int a[display(3)] = {1, 2, 3, 4};
    return 0;
}
```
常量表达式函数的返回值必须是常量表达式的原因很简单，如果想在程序编译阶段获得某个函数返回的常量，则该函数的 return 语句中就不能包含程序运行阶段才能确定值的变量。

# constexpr 修饰类的构造函数
对于 C++ 内置类型的数据，可以直接用 constexpr 修饰，但如果是自定义的数据类型（用 struct 或者 class 实现），直接用 constexpr 修饰是不行的。

```cpp
// 无法通过编译,编译器会抛出“constexpr不能修饰自定义类型”的异常。
#include <iostream>
using namespace std;

// 自定义类型的定义
constexpr struct myType {
    const char *name;
    int age;
    // 其它结构体成员
};

int main() {
    constexpr struct myType mt{"zhangsan", 10};
    cout << mt.name << " "  << mt.age << endl;
    return 0;
}
```
当我们想自定义一个可产生常量的类型时，正确的做法是在该类型的内部添加一个常量构造函数。

```cpp
#include <iostream>

using namespace std;
// 自定义类型
struct myType {
    constexpr myType(char *name, int age):name(name), age(age){};
    const char *name;
    int age;
};

int main() {
    constexpr struct myType mt{"zhangsan", 10};
    cout << mt.name << " " << mt.age << endl;
    return 0;
}
```
注意，constexpr 修饰类的构造函数时，要求该构造函数的函数体必须为空，且采用初始化列表的方式为各个成员赋值时，必须使用**常量表达式**。

前面提到，constexpr 可用于修饰函数，而类中的成员方法完全可以看做是“位于类这个命名空间中的函数”，所以 constexpr 也可以修饰类中的成员函数，只不过此函数必须满足前面提到的 4 个条件。

```cpp
#include <iostream>
using namespace std;

// 自定义类型的定义
class myType {
public:
    constexpr myType(const char *name, int age):name(name), age(age){};
    constexpr const char*getname() {
        return name;
    }

    constexpr int getage() {
        return age;
    }
private:
    const char *name;
    int age;
};

int main() {
    constexpr struct myType mt{"zhangsan", 10};
    constexpr const char *name = mt.getname();
    constexpr int age = mt.getage();

    cout << name << " " << age << endl;

    return 0;
}
```
注意，C++11 标准中，不支持用 constexpr 修饰带有 virtual 的成员方法。

# constexpr 修改模板函数

C++11 语法中，constexpr 可以修饰模板函数，但由于模板中类型的不确定性，因此模板函数实例化后的函数是否符合常量表达式函数的要求也是不确定的。

针对这种情况下，C++11 标准规定，如果 constexpr 修饰的模板函数实例化结果不满足常量表达式函数的要求，则 constexpr 会被自动忽略，即该函数就等同于一个普通函数。

```cpp
#include <iostream>
using namespace std;

// 自定义数据的定义
struct myType {
    const char *name;
    int age;
};

// 模板函数
template <typename T>
constexpr T display(T t) {
    return t;
}

int main() {
    struct myType stu{"zhangsan", 10};
    // 普通函数
    struct myType ret = display(stu);

    // 常量表达式函数
    constexpr int ret1 = display(10);
    cout << ret1 << endl;

    return 0;
}
```

# constexpr和const区别

constexpr 是 C++ 11 标准新添加的关键字，在此之前（C++ 98/03标准）只有 const 关键字，其在实际使用中经常会表现出两种不同的语义。举个例子：

C++ 11标准中，为了解决 const 关键字的双重语义问题，保留了 const 表示“只读”的语义，而将“常量”的语义划分给了新添加的 constexpr 关键字。因此 C++11 标准中，建议将 const 和 constexpr 的功能区分开，即凡是表达“只读”语义的场景都使用 const，表达“常量”语义的场景都使用 constexpr。

```cpp
#include <iostream>
#include <array>
using namespace std;

void disl_1(const int x) {
    // 错误，x是只读的变量
    array<int, x> myarr{1, 2, 3, 4, 5};
    cout << myarr[1] << endl;
}

void dis_2() {
    const int x = 5;
    array<int , x> myarr{1,2,3,4,5};
    cout << myarr[1] << endl;
}

int main() {
    dis_1(1);
    dis_2();
}
```
可以看到，dis_1() 和 dis_2() 函数中都包含一个 const int x，但 dis_1() 函数中的 x 无法完成初始化 array 容器的任务，而 dis_2() 函数中的 x 却可以。

这是因为，dis_1() 函数中的“const int x”只是想强调 x 是一个只读的变量，其本质仍为变量，无法用来初始化 array 容器；而 dis_2() 函数中的“const int x”，表明 x 是一个只读变量的同时，x 还是一个值为 5 的常量，所以可以用来初始化 array 容器。

C++ 11标准中，为了解决 const 关键字的双重语义问题，保留了 const 表示“只读”的语义，而将“常量”的语义划分给了新添加的 constexpr 关键字。因此 C++11 标准中，建议将 const 和 constexpr 的功能区分开，即凡是表达“只读”语义的场景都使用 const，表达“常量”语义的场景都使用 constexpr。

在上面的实例程序中，dis_2() 函数中使用 const int x 是不规范的，应使用 constexpr 关键字。

有读者可能会问，“只读”不就意味着其不能被修改吗？答案是否定的，“只读”和“不允许被修改”之间并没有必然的联系，举个例子：

```cpp
#include <iostream>
using namespace std;
int main() {
    int a = 10;
    const int &con_b = a;
    cout << con_b << endl;
    a = 20;
    cout << con_b << endl;
}
```
可以看到，程序中用 const 修饰了 con_b 变量，表示该变量“只读”，即无法通过变量自身去修改自己的值。但这并不意味着 con_b 的值不能借助其它变量间接改变，通过改变 a 的值就可以使 con_b 的值发生变化。

```cpp
// 在大部分实际场景中，const 和 constexpr 是可以混用的
const int a = 5 + 4;
constexpr int a = 5 + 4;
```

它们是完全等价的，都可以在程序的编译阶段计算出结果。但在某些场景中，必须明确使用 constexpr，例如：

```cpp
#include <iostream>
#include <array>

using namespace std;
constexpr int sqr1(int arg) {
    return arg * arg;
}

const int sqr2(int arg) {
    return arg * arg;
}

int main() {
    array<int, sqr1(10)> mylist1; // 可以，因为sqr1时constexpr函数
    array<int, sqr2(10)> mylist1; // 不可以，以为sqr2不是constexpr函数
    return 0;
}
```
其中，因为 sqr2() 函数的返回值仅有 const 修饰，而没有用更明确的 constexpr 修饰，导致其无法用于初始化 array 容器（只有常量才能初始化 array 容器）。

总的来说在 C++ 11 标准中，const 用于为修饰的变量添加“只读”属性；而 constexpr 关键字则用于指明其后是一个常量（或者常量表达式），编译器在编译程序时可以顺带将其结果计算出来，而无需等到程序运行阶段，这样的优化极大地提高了程序的执行效率。
