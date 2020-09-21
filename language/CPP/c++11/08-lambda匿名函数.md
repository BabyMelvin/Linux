# C++11 lambda匿名函数
lambda 用来表示一种匿名函数.

lambda匿名函数的定义：

```
[外部变量访问方式说明符](参数) mutable noexcept/throw()->返回值类型
{
    函数体;
};
```

* 1.`[]`说明当前是lambda表达式，不能省略.方括号内可以注明当前lambda函数体可以使用哪些外部变量.
    * 外部变量，和当前lambda表达式位于同一作用域内的所有局部变量.
* 2.`(参数)`可以传递参数，如果不需要传递参数，可以省略小括号
* 3.`mutable`关键字可以省略，使用则前面小括号不能省略.
    * 默认，对于值传递方式引入外部变量，不允许lambda白大师内部修改他们(可理解为const常量)
    * 如果想修改，可必须使用mutable关键字
* 4.`noexcept/throw()`,可以省略，使用小括号不能省略.
    * noexcept指定不会抛出任何异常
    * throw抛出异常类型 如果noexcept 有指定了throw异常，或者throw 无法捕获则会执行失败
* 5.`->`返回值类型
    * 如果lambda函数体只有一个return语句，或者该函数返回void,编译器可以推断，省略不写

lambda表达式可以使用任意一个全局变量，必要时可以直接修改它们的值

最简单的匿名函数`[]{}`

lambda匿名函数中的外部变量：

* `[]` 空括号表示当前lambda匿名函数中不导入任何外部变量
* `[=]`只有一个`=`等号，表示以值传递的方式导入所有外部变量
* `[&]`只有一个`&`符号，表示以应用方式导入所有外部变量
* `[val1, val2, ...]`表示值形式导入val1, val2等指定外部变量，同时多个变量之间没有先后次序
* `[&val1, &val2, ...]`表示以引用方式导入val1, val2等指定外部变量
* `[val, &val2, ...]`2种方式还是可以混合使用，没有前后次序
* `[=, &val1, ...]`，表示除val1以引用传递方式导入外，其他外部变量都以值形式的方式导入
* `[this]` 表示值传递的方式导入this指针

不允许相同方式导入多次,`[=, val1]`中val1先后被值方式导入2次，这是非法的.

# 2.lambda定义和使用

```cpp
#include <iostream>
#include <algorithm>

using namespace std;

int main()
{
    int num[4] = {4, 3, 2, 1};

    // 对a数组中的元素进行排序
    sort(num, num + 4, [=](int x, int y)->bool{return x < y;});
    for (int n: num) {
        cout << n << " ";
    }

    // 可以手动设置一个名称
    auto display = [](int a, int b)->void{cout<<a<<" "<<b;};
    // 使用lambda函数
    display(10, 20);
    return 0;
}
```

值传递和引用传递的区别

```cpp
#include <iostream>
using namespace std;

// 全局变量
int all_num = 0;

int main ()
{
    // 局部变量
    int num_1 = 1;
    int num_2 = 2;
    int num_3 = 3;
    cout << "lambda1:\n" 

    auto lambda1 = [=] {
        // 全局变量可以直接访问或修改
        all_num = 10;

        // 函数体只能使用外部变量，而无法对它们进行修改
        cout << num_1 << " "
             << num_2 << " "
             << num_3 << endl;

    };

    lambda1();

    cout << all_num << end;

    cout << "lambda2:\n";
    auto lambda2 = [&] {
        all_num = 100;
        num_1 = 10;
        num_2 = 20;
        num_3 = 30;
        cout << num_1 << " "
             << num_2 << " "
             << num_3 << endl;


    };
    lambda2();
    cout << all_num << end;

    return 0;
}
```

lambda1 匿名函数是以 `[=]` 值传递的方式导入的局部变量，这意味着默认情况下，此函数内部无法修改这 3 个局部变量的值，但全局变量 all_num 除外。相对地，lambda2 匿名函数以 `[&]` 引用传递的方式导入这 3 个局部变量，因此在该函数的内部不就可以访问这 3 个局部变量，还可以任意修改它们。同样，也可以访问甚至修改全局变量。

如果我们想在 lambda1 匿名函数的基础上修改外部变量的值，可以借助`mutable`关键字

```cpp
auto lambda1 = [=]() mutable {
        num_1 = 10;
        num_2 = 20;
        num_3 = 30;
        cout << num_1 << " "
             << num_2 << " "
             << num_3 << endl;
};
```
由此，就可以在 lambda1 匿名函数中修改外部变量的值。但需要注意的是，这里修改的仅是 num_1、num_2、num_3 **拷贝的那一份的值**，**真正外部变量的值并不会发生改变**。

3.执行抛出异常类型

```cpp
#include <iostream>
using namespace std;

int main()
{
    auto except = []() throw(int) {
        throw 10;
    };

    try {
        except();
    } catch (int) {
        cout << "捕获到了整形异常";
    }

    return 0;
}
```
