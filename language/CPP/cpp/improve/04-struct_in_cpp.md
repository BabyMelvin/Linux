# struct
C++ 中保留了C语言的 struct 关键字，并且加以扩充。在C语言中，struct 只能包含成员变量，不能包含成员函数。而在C++中，struct 类似于 class，既可以包含成员变量，又可以包含成员函数。

C++中的 struct 和 class 基本是通用的，唯有几个细节不同：

* 使用 class 时，类中的成员默认都是 private 属性的；而使用 struct 时，结构体中的成员默认都是 public 属性的。
* class 继承默认是 private 继承，而 struct 继承默认是 public 继承（《C++继承与派生》一章会讲解继承）。
* class 可以使用模板，而 struct 不能（《模板、字符串和异常》一章会讲解模板）
