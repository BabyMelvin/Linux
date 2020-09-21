# 1.decltype类型推导

c++11新增加关键字 decltype(declare type)和auto一样，在编译期间自动类型推导

```cpp
auto varName = value;

// exp是一个表达式
decltype(exp) varname = value;
```

* auto根据右边value推导出类型
* decltype根据exp推导出类型，和右边value没有关系
* auto变量必须初始化，decltype没有要求
* 避免exp使用void类型返回

## 1.1 decltype推导规则

type(exp) 获取类型时，编译器将根据以下三条规则得出结果：

* 如果 exp 是一个不被括号( )包围的表达式，或者是一个类成员访问表达式，或者是一个单独的变量，那么 decltype(exp) 的类型就和 exp 一致，这是最普遍最常见的情况。
* 如果 exp 是函数调用，那么 decltype(exp) 的类型就和函数返回值的类型一致。
* 如果 exp 是一个**左值**，或者被括号( )包围，那么 decltype(exp) 的类型就是 exp 的引用；假设 exp 的类型为 T，那么 decltype(exp) 的类型就是 T&。

```cpp
// 1.exp普通表达式
#include <string>
using namespace std;

class Student {
public:
    static int total;
    string name;
    int age;
    float score;
};

int Student::total = 0;

int main () {
    int n = 0;
    const int &r = n;
    Student stu;

    decltype(n) a = n; // n为int类型，a被自动推导
    decltype(r) b = n; // r为const int & 类型，b被推导为const int&类型
    decltype(Student::total) c = 0; //total为类Student的一个int类成员变量，c推导为int类型
    decltype(stu.name) url = "http://c.biancheng.net/cplus/"; // total为类Student一个string类型成员变量，url被推导为string类型

    return 0;
}
```

```cpp
// 2.exp为函数调用
int &func_int_r(int, char); //返回值int &
int &&func_init_rr(void); // 返回类型int&&
int func_int(double); // 返回值为int

const int& func_cint_r(int, int, int); // 返回值为const int &
const int &&func_cint_rr(void); // 返回值为const int &&
//decltype类型推导
int n = 100;
decltype(func_int_r(100, 'A')) a = n;  //a 的类型为 int&
decltype(func_int_rr()) b = 0;  //b 的类型为 int&&
decltype(func_int(10.5)) c = 0;   //c 的类型为 int
decltype(fun_cint_r(1,2,3))  x = n;    //x 的类型为 const int &
decltype(func_cint_rr()) y = 0;  // y 的类型为 const int&&
```

```cpp
// exp是左值，或被()包围
using namespace std;

class Base{
public:
    int x;
};

int main(){
    const Base obj;

    //带有括号的表达式
    decltype(obj.x) a = 0;  //obj.x 为类的成员访问表达式，符合推导规则一，a 的类型为 int
    decltype((obj.x)) b = a;  //obj.x 带有括号，符合推导规则三，b 的类型为 int&。

    //加法表达式
    int n = 0, m = 0;
    decltype(n + m) c = 0;  //n+m 得到一个右值，符合推导规则一，所以推导结果为 int
    decltype(n = n + m) d = c;  //n=n+m 得到一个左值，符号推导规则三，所以推导结果为 int&

    return 0;
}
```

# 2. decltpe实际使用

auto只用于类静态成员，不能用于类的非静态成员，使用decltype:

```cpp
#include <vector>
using namespace std;

template <typename T>
class Base {
public:
    void func(T& container) {
        m_it = container.begion();
    }

private:
    typename T::iterator m_it; // 注意这里
};

int main() {
    const vector<int> v;
    Base<const vector<int>> obj;
    obj.func(v);

    return 0;
}
```
单独看 Base 类中 m_it 成员的定义，很难看出会有什么错误，但在使用 Base 类的时候，如果传入一个 const 类型的容器，编译器马上就会弹出一大堆错误信息。原因就在于，`T::iterator`并不能包括所有的迭代器类型，当 T 是一个 const 容器时，应当使用 const_iterator。

要想解决这个问题，在之前的 C++98/03 版本下只能想办法把 const 类型的容器用模板特化单独处理，增加了不少工作量，看起来也非常晦涩。但是有了 C++11 的 decltype 关键字，就可以直接这样写：

```cpp
template <typename T>
class Base {
public:
    void func(T& container) {
        m_it = container.begion();
    }

private:
    decltype(T().begin()) m_it;  // 注意这里
};
```
注意，有些低版本的编译器不支持T().begin()这种写法，以上代码我在 VS2019 下测试通过，在 VS2015 下测试失败。

# 3.auto和decltype区别
## 3.1对cv的处理
const 和 volatile 关键字

* decltype保留cv限定符，而auto有可能会去掉cv限定符

```cpp
//非指针非引用类型
const int n1 = 0;

auto n2 = 10;
n2 = 99;  //赋值不报错

decltype(n1) n3 = 20;
n3 = 5;  //赋值报错

//指针类型
const int *p1 = &n1;

auto p2 = p1;
*p2 = 66;  //赋值报错

decltype(p1) p3 = p1;
*p3 = 19;  //赋值报错
```

## 3.2 对引用处理
当表达式的类型为引用时，auto 和 decltype 的推导规则也不一样；decltype 会保留引用类型，而 auto 会抛弃引用类型，直接推导出它的原始类型。请看下面的例子：

```cpp
#include <iostream>
using namespace std;

int main() {
    int n = 10;
    int &r1 = n;

    //auto推导
    auto r2 = r1;
    r2 = 20;
    cout << n << ", " << r1 << ", " << r2 << endl;

    //decltype推导
    decltype(r1) r3 = n;
    r3 = 99;
    cout << n << ", " << r1 << ", " << r3 << endl;

    return 0;
}
```

输出为：

```
10, 10, 20
99, 99, 99
```
