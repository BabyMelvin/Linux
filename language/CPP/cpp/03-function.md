## inline内联函数

C++中的const常量可以替代宏常数定义，内联函数可用来宏代码片段。
**注意**：内联函数声明时inline关键字必须和函数定义结合在一起，否则编译器会直接忽略内联请求。

* 内联函数由编译器处理，直接将编译后的函数体插入调用的地方
* 宏代码片段由预处理器处理，进行简单的文本替换，没有任何编译过程。

* C++现代编译器能够进行编译优化，因此一些函数即使没有Inline声明，也可能被编译器内联
* 一些现代编译器提供扩展语法，对函数进行强制内联`__attribute__((always_inline))`属性

C++中内联编译限制：

* 不存在任何形式的循环语句
* 不能存在过多的条件判断语句
* 函数体不能过庞大(函数体执行开销远大于压栈开销，内联则无意义)
* 不能对函数进行取址操作
* 函数内联声明必须在调用语句之前（*）

c++内联结论：

* 内联函数在编译时直接将函数体插入函数调用的地方
* inline只是一个请求，编译器不一定允许这种请求
* 内联函数省去了普通函数调用时压栈，跳转和返回的开销

