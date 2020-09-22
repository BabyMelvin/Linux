# shared_ptr智能指针 

* 有些内存资源已经被释放，但指向它的指针并没有改变指向（成为了野指针），并且后续还在使用；
* 有些内存资源已经被释放，后期又试图再释放一次（重复释放同一块内存会导致程序运行崩溃）；
* 没有及时释放不再使用的内存资源，造成内存泄漏，程序占用的内存资源越来越多。

C++11 新标准在废弃 auto_ptr 的同时，增添了 unique_ptr、shared_ptr 以及 weak_ptr 这 3 个智能指针来实现堆内存的自动回收.

智能指针和普通指针的用法是相似的，不同之处在于，智能指针可以在适当时机自动释放分配的内存。也就是说，使用智能指针可以很好地避免“忘记释放内存而导致内存泄漏”问题出现。

# C++11 shared_ptr智能指针
hared_ptr 也不例外。`shared_ptr<T>`（其中 T 表示指针指向的具体数据类型）的定义位于`<memory>`头文件，并位于 std 命名空间中，因此在使用该类型指针时，程序中应包含如下 2 行代码：

```cpp
#include <memory>
using namespace std;
```
unique_ptr、weak_ptr 不同之处在于，**多个shared_ptr智能指针可以共同使用同一块堆内存**。并且，由于该类型智能指针在实现上采用的是引用计数机制，即便有一个 shared_ptr 指针放弃了堆内存的“使用权”（引用计数减 1），也不会影响其他指向同一堆内存的 shared_ptr 指针（只有引用计数为 0 时，堆内存才会被自动释放）。

## 1.shared_ptr智能指针的创建

`shared_ptr<T>` 类模板中，提供了多种实用的构造函数，这里给读者列举了几个常用的构造函数（以构建指向 int 类型数据的智能指针为例）。

1).通过如下 2 种方式，可以构造出`shared_ptr<T>`类型的空智能指针：

```cpp
std::shared_ptr<int> p1;   // 不传入任何参数
std::shared_ptr<int> p2(nullptr); // 传入空指针nullptr
```

注意，空的shared_ptr指针，其初始引用计数为0，而不是1.

2). 在构建shared_ptr智能指针，可以其明确指向

```cpp
std::shared_ptr<int> p3(new int(10));
```
由此，我们就成功构建了一个shared_ptr智能指针，其指向一块存有10这个int类型数据的堆内存空间。

同时，C++11 标准中还提供了 `std::make_shared<T>`**模板函数**，其可以用于初始化 shared_ptr 智能指针，例如：

```cpp
// 和上面创建p3的方式完全相同
std::shared_ptr<int> p3 = std::make_shared<int>(10);
```

3)`shared_ptr<T>`模板还提供有相应的拷贝构造函数和移动构造函数

```cpp
// 调用拷贝构造函数
std::shared_ptr<int> p4(p3); // 或者 std::shared_ptr<int> p4 = p3;

// 调用移动构造函数
std::shared_ptr<int> p5(std::move(p4)); // 或者std::shared_ptr<int> p5 = std::move(p4);
```
如上所示，p3 和 p4 都是 shared_ptr 类型的智能指针，因此可以用 p3 来初始化 p4，由于 p3 是左值，因此会调用拷贝构造函数。

需要注意的是

* 如果 p3 为空智能指针，则 p4 也为空智能指针，其引用计数初始值为 0；
* 反之，则表明 p4 和 p3 指向同一块堆内存，同时该堆空间的引用计数会加 1。

`std::move(p4)`来说，该函数会强制将p4转换成对应的右值，因此初始化p5调用的是`移动构造函数`。另外和调用拷贝构造函数不同，用`std::move(p4)`初始化p5，会使得 p5拥有了p4的堆内存，而p4 则变成了**空智能指针**。

**注意**： **同一普通指针**不能同时为多个shared_ptr对象赋值，否则会导致程序发生异常

```cpp
int *ptr = new int;
std::shared_ptr<int> p1(ptr);
std::shared_ptr<int> p2(ptr); // 错误
```
4).初始化 shared_ptr 智能指针时，还可以自定义所指堆内存的释放规则，这样当堆内存的引用计数为 0 时，会**优先调用我们自定义的释放规则**。

在某些场景中，自定义释放规则是很有必要的。比如，对于**申请的动态数组**来说，shared_ptr 指针默认的释放规则是不支持释放数组的，只能自定义对应的释放规则，才能正确地释放申请的堆内存。

对于申请的动态数组，释放规则可以使用 C++11 标准中提供的`default_delete<T>`模板类，我们也可以自定义释放规则：

```cpp
// 指定default_delete 作为释放规则
std::shared_ptr<int> p6(new int[10], std::default_delete<int[]>());

// 自定义释放规则
void deleteInt(int *p) {
    delete []p;
}

// 初始化智能指针，并自定义释放规则
std::shared_ptr<int> p7(new int[10], deleteInt);

// 或者使用lambda表达式
std::shared_ptr<int> p7(new int[10], [](int *p){delete []p;});
```

## 2.`shared_ptr<T>`模板类提供的成员方法

为了方便用户使用 shared_ptr 智能指针，`shared_ptr<T>`模板类还提供有一些实用的成员方法，它们各自的功能如表 1 所示。

* `operator=()`:重载符号值，使得同一类型的shared_ptr智能指针可以相互赋值
* `operator*()`:重载`*`号，获取当前shared_ptr智能指针指针对象的数据
* `operator->()`:重载`->`号
* `swap()`:交换2个相同类型shared_ptr智能指针的内容
* `reset()`:
    * 当函数没有实参时，该函数会使当前 shared_ptr 所指堆内存的引用计数减 1，同时将当前对象重置为一个空指针；
    * 当为函数传递一个新申请的堆内存时，则调用该函数的 shared_ptr 对象会获得该存储空间的所有权，并且引用计数的初始值为 1。
* `get()`:获得shared_ptr对象内部包含的普通指针
* `use_count()`:返回同当前shared_ptr对象(包括它)指向相同的所有shared_ptr对象的数量
* `unique()`：判断当前 shared_ptr 对象指向的堆内存，是否不再有其它 shared_ptr 对象再指向它。
* `operator bool()`:判断当前 shared_ptr 对象是否为空智能指针，如果是空指针，返回 false；反之，返回 true。
* 除此之外，C++11 标准还支持同一类型的 shared_ptr 对象，或者 shared_ptr 和 nullptr 之间，进行 `==，!=，<，<=，>，>= `运算。

# 实例

```cpp
#include <iostream>
#include <memory>
using namespace std;

int main() {
    // 构建2个智能指针
    std::shared_ptr<int> p1(new int(10));
    std::shared_ptr<int> p2(p1);

    // 输出p2指向的数据
    cout << *p2 << endl;

    p1.reset();  // 引用计数减1，p1为空指针
    if (p1) {
        cout << " p1 不为空" << endl;
    } else {
        cout << " p1 为空" << endl;
    }

    // 以上操作并不影响p2
    cout << *p2 << endl;

    // 判断当前和p2同指向的智能指针有多少个
    cout << p2.use_count() << endl;

    return 0;
}
```

```
10
p1 为空
10
1
```
