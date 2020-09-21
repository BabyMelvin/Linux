# auto

c++11，使用它做自动类型推导，编译器在编译器件自动推导出变量的类型

```cpp
// auto 只是占位符，编译期间被真正类型取代
// c++必须申明类型，只是编译自己推导出来
auto name = value;

auto n = 10;
auto f = 12.8;

// 可连续多个变量
int n = 20;
auto *p = &n, m = 99;
```

# 1.auto高级用法

```cpp
// auto 和其他结合使用
int x = 0;
auto *p = &x;   // p为int*,auto 推导为int
auto p1 = &x;   // p1为int*, auto 推导为int*
```
auto 和const结合使用

```cpp
int x = 0;
const auto n = x; // n为const int, auto 被推导为int
auto f = n; // f 为const int, auto 被推导为int
```

注意：

* 1.使用auto必须对变量进行初始化
* 2.auto不能在函数参数中使用
* 3.auto不能作用于类的非静态成员变量
* 4.auto关键字不能定义数组
* 5.auto不能作用于模板参数

```cpp
// 下面是错误的
template <typename T>
class A {
    // TODO
};

int main() {
    A<int> C1;
    A<auto> C2 = C1; //todo 错误
    return 0;
}
```

# 2.auto应用
# 2.1 auto定义迭代器
auto 来定义stl的迭代器

```cpp
// 不同容器迭代器不同类型
#include <vector>
using namespace std;

int main() {
    vector <vector<int>> v;
    vector<vector<int>>::iterator i = v.begin();
    return 0;
}

//TODO 转换之后

int main() {
    vector<vector<int>> v;
    auto i = v.begin(); //使用auto代替具体的类型

    return 0;
}
```

# 2.2 auto用于泛型编程

```cpp
#include <iostream>
using namespace std;

class A {
public:
    static int get(void) {
        return 100;
    }
};

class B {
public:
    static const char *get(void) {
        return "http://c.bincheng.net/cplus";
    }
};

template <typename T>
void func(void) {
    auto val = T::get();
    cout << val << endl;
}

template <typename T1, typename T2> //额外增加一个模板参数T2
void funcNoAuto(void) {
    T2 val = T1::get();
    cout << val << endl;
}

int main(void) {
    func<A>();
    func<B>();

    // 不用auto
    funcNoAuto<A, int>();
    funcNoAuto<B, const char*>();

    return 0;
}
```
