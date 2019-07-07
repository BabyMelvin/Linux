dlopen几个注意点：

1. dlopen相当于一个独立插
2. dlopen和主程序的地址空间是独立的。同样的变量也不能进行共享数据


dlopen独立的特性，有几种解决方法：

1. 通过dlopen之后dlsym进行函数映射传递交互(类似线程的调用)
2. dlsym也可以进行映射全局变量名
3. 进行extern方式(待验证)



# extern方式

共享函数中的全局变量，在共享对象中用extern声明它：

动态库使用：

```
MACOSX_DEPLYMENT_TARGET=10.3 LD -dylib -undefined dynamic_lookup -o multiply.so multiply.o
```
或

```
MACOSX_DEPLYMENT_TARGET=10.3 libtool的-dynamic LD -dylib -undefined dynamic_lookup -o multiply.so multiply.o
```