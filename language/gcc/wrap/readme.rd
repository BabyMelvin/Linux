# –wrap=symbol

函数名为“__wrap_symbol”，且称其为包装函数

* "symbol”是一个函数名

执行过程：

* 当调用“symbol”函数时，如果“symbol”函数未定义的话就会调用“__wrap_symbol”函数;
* “__real_symbol”也是个相关的函数，当其只声明不定义的话，我们对其的调用将调用真正的“symbol”函数；
* 当然，我们还要添加编译链接参数：-Wl,–wrap,symbol
