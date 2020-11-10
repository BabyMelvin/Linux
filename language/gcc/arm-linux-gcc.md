# 1.arm-linux-gcc 选项
可执行程序需要：预处理、编译、汇编和连接。

* 1.预处理：`#`开头成为预处理命令。预处理将**要包含(include)的文件插入源文件中**、**将宏展开**、**根据条件编译命令选择要使用的代码**，最后将这些东西输出到一个`.i`文件中等待进一步处理。用到`arm-linux-cpp`工具。
* 2.编译：将`C/C++`代码(上面`.i`文件)翻译成汇编代码，用到工具`ccl`
* 3.汇编：将第2步输出汇编代码翻译成符合一定格式的机器代码，Linux系统表现为ELF目标文件(OBJ文件)，用到`arm-linux-as`。反汇编：指将机器代码转换为汇编代码，调试用到。
* 4.连接：将生成的OBJ文件和系统库OBJ文件、库文件连接起来，最终生成了可以在特定平台运行的可执行文件，用到`arm-linux-ld`.

编译器和链接器:
* 编译器：`.i`:预处理后文件。`.s`或`.S`汇编语言程序
* 连接器：`.o`:目标文件(Object file,OBJ文件)。`.a`:归档库文件（Archive file）

## 1.1 总体选项

* `gcc -v`:显示制作GCC工具自身时命令；同时显示编译器驱动程序、预处理器、编译器的版本号。
	* TARGET:`x86_64-linux-gnu`
	* Configured with:include和lib等
	* gcc version
* `-o file`:执行输出文件file。


```c
//hello.c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	printf("hello world\n");
	return 0;
}
```

* `-E`:预处理后停止，不进行编译。`gcc -E hello.c -o hello.i`
	* include声明要的h文件
	* 去掉宏
	* 选择要编译的代码内容
* `-S`:编译后停止，不进行汇编。:`gcc -S -o hello.s hello.c`
* `-c`:预处理、编译和汇编源文件，但不作为连接，编译根据源文件生成OBJ文件。GCC通过`.o`替换原`.c`,`.i`,`.s`等。`gcc -c hello.o hello.c`
* 连接成可执行文件:`gcc hello.o -o hello`


## 1.2警告选项

当gcc在编译不符合ANSI/ISO C语言标准的源代码时，如果加上了`-pedantic`选项，那么使用了扩展语法的地方将产生相应的警告信息：`-pedantic`编译选项并不能保证被编译程序与ANSI/ISO C标准的完全兼容，它仅仅用来帮助Linux程序员离这个目标越来越近。

* `-Wall`:打开所有需要注意的警告信息，比如没有指定类型声明、声明就使用函数、局部变量声明但未使用
*在处理警告方面，另一个常用的编译选项是`-Werror`，它要求gcc将所有的警告当成错误进行处理，这在使用自动编译工具(如make等)时非常有用。

## 1.3调试选项(Debugging Option)

* `-g`

## 1.4优化选项(Optimization Option)

> gcc -O2 -c -o main.o main.c

* `-O`或`-O1`:优化编译过程占用稍微多的时间和相当多的内存。使用这个选项`-fthread-jump`和`-fdefer-pop`选项被打开。
* `-O2`:多优化一些。既增加编译时间，也提高了生成代码的效果。
* `-O3`:优化更多。打开了`-finline-fucntions`选项。
* `-O0`:不优化

借助Linux提供的time命令，可以大致统计出该程序在运行时所需要的时间：`time main`

## 1.5连接器选项(Linker Option)
> 连接OBJ文件，输出可执行文件或库文件

* 1.`object-file-name`:某些文件没有明确后缀，GCC认为他们是OBJ文件或库文件。连接操作，OBJ文件成为连机器的输入文件。`main.o`,`sub.o`等输入文件
* 2.`-llibrary`:连接名为library的库文件。连接器在标准搜索目录中寻找这个库文件，库文件的真正名字是`liblibrary.a`。出列一些标准目录外，还包括用户以`-L`选项执行的路径。一般用这个方法来找到文件的库文件(由Obj组成的归档文件Archive file).
	* 连接处理归档文件的方法：扫描归档文件，寻找某些成员，这些成员的符号目前已经被引用，不过还没有被定义。**但是**，连接器找到是普通OBJ文件，不是库文件按普通方式连接进来。
    * 注意：加`-l`和普通文件区别，`-l`会扩展为实际名称和搜索一些目录。
* 3.`-nostdlib`:不连接系统标准启动文件和标准库文件，只把指定的文件传递给连接器。这个选项常用于编译内核、bootloader程序，不需要启动文件、标准库文件。
* 4.`-static`:支持动态链接的系统上，阻止链接共享库。
* 5.`-shared`:生成一个共享OBJ文件，它可以和其他OBJ文件连接可执行文件。

## 1.6目录选项(Directory Option)
执行搜索路径，用于查找头文件，库文件，或编译器的某些成员。

* `-Idir`:在头文件的搜索路径列表中添加dir目录。
	* 如果以`#include<>`包含文件，则只在标准库目录开始搜索(包含使用`-Idir`选项定义的目录);
	* 如果以`#include""`包含文件，则先从用户的工作目录开始搜索，再搜索标准库目录。
* `-I-`:`-I-`不影响使用系统标准目录。
	* 在`-I-`前面使用`-I`选项指定搜索路径只适用于`#include "file"`情况，不能用来搜索`#include<file>`包含头文件。
	* 用`-I`位于`-I-`后面能正常
* `-Ldir`:在`-l`选项搜索路径列表中添加dir目录

```makefile
gcc -c -o sub.o sub.c
gcc -shared -o -fPIC libsub.a sub.o
gcc -L. -o test main.o -lsub
```

## 1.7 加速

在将源代码变成可执行文件的过程中，需要经过许多中间步骤，包含预处理、编译、汇编和连接。这些过程实际上是由不同的程序负责完成的。大多数情况下gcc可以为Linux程序员完成所有的后台工作，自动调用相应程序进行处理。

这样做有一个很明显的缺点，就是gcc在处理每一个源文件时，最终都需要生成好几个临时文件才能完成相应的工作，从而无形中导致处理速度变慢。例如，gcc在处理一个源文件时，可能需要一个临时文件来保存预处理的输出，一个临时文件来保存编译器的输出，一个临时文件来保存汇编器的输出，而读写这些临时文件显然需要耗费一定的时间。当软件项目变得非常庞大的时候，花费在这上面的代价可能会变得很大。

解决的办法是，使用Linux提供的一种更加高效的通信方式—— 管道。它可以用来同时连接两个程序，其中一个程序的输出将直接作为另一个程序的输入，这样就可以避免使用临时文件，但编译时却需要消耗更多的内存。

在编译过程中使用管道是由gcc的`-pipe`选项决定的。下面的这条命令就是借助gcc的管道功能来提高编译速度的：

```
gcc -pipe hello.c -o hello
```

# 2.`__attribute__`选项
`__attribute__` 可以用来设置 **函数属性**, **变量属性** 和 **类型属性**, **只与声明一起使用**. **即使函数是定义在同一文件内**, **也需要额外提供声明才能生效**. 写法为`__attribute__((arg1, arg2, arg3…))`, 这里列举一些用比较常用的属性:

## 2.1 format
format 被用来描述函数的参数形式, 支持的参数形式有printf, scanf, strftime 和 strfmon,比较常用的是 printf 形式, format 的语法为:`__attribute__((format(archetype, string-index, first-to-check)))`

* archetype 为参数形式
* string-index 是指函数的格式化字符串在所有参数中所处的位置
* first-to-check 是指第一个格式化参数的位置

例如：

```c
void myprintf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
```
以上声明表示`myprint()`函数的第一个参数为格式化字符串, 第二个及往后的都是格式化参数, 这样以下调用在` gcc -Wformat` 选项中可能会产生警告:

```c
// 编译检测
myprint("%s", 6); // warning: format ‘%s’ expects type ‘char *’, but argument 2 has type ‘int’
myprint("%d", 7); // ok
myprint("%d%s%d", 7, 3); // test.c:10: warning: format ‘%s’ expects type ‘char *’, but argument 3 has type ‘int’
						 // test.c:10: warning: too few arguments for format
```

# 2.2 const
const属性告诉编译器该函数**调用一次后缓存返回结果**,以后的调用直接使用缓存的值

## 2.3 noreturn
例如以下代码使用`-Wall`选项编译时会产生警告: warning: control reaches end of non-void function

```c
extern void myexit{exit()};
int test(int n) {
if ( n > 0 ) {
	myexit();
} // 控制逻辑分支没有返回值
else
	return 0;
}
```

而将`myexit()`的声明改为:`extern void myexit(exit()) __attribute__((noreturn))`则不会产生上面的警告了.

## 2.4 aligned
设置类型所使用的对齐方式,

```c
#include
struct a {
	char b;
	short c;
};

struct a4 {
	char b;
	short c;
} __attribute__((aligned(4)));

struct a8 {
	char b;
	short c;
} __attribute__((aligned(8)));
struct ap {
	char b;
	short c;
} __attribute__((packed));
int main() {
	printf("sizeof(char) = %dn",sizeof(char));
	printf("sizeof(short) = %dn",sizeof(short));
	printf("sizeof(a) = %dn",sizeof(struct a));
	printf("sizeof(a4) = %dn",sizeof(struct a4));
	printf("sizeof(a8) = %dn",sizeof(struct a8));
	printf("sizeof(ap) = %dn",sizeof(struct ap));
}
```
运行结果为

```
sizeof(char) = 1
sizeof(short) = 2
sizeof(a) = 4
sizeof(a4) = 4
sizeof(a8) = 8
sizeof(ap) = 3
```

## 2.5 no_instrument_function
关于这个参数的使用首先要解释一下 gcc 的 `-finstrument-functions` 选项, 当 GCC 使用这个选项编译代码的时候会在**每一个用户自定义函数中添加两个函数调用**:

```c
void __cyg_profile_func_enter(void *this, void *callsite);
void __cyg_profile_func_exit(void *this, void *callsite);
```
这两个函数是在 glibc 内部声明的, 可以由用户自己定义实现, `__cyg_profile_func_enter()`在进入函数的时候调用, `void __cyg_profile_func_exit()`在函数退出的时候调用. 第一个参数this指向当前函数地址, 第二个参数 callsite 指向上一级函数地址,举个例子来说明一下:

```c
#include <stdio.h>
#define debug_print(fmt, args...) \
	do { \
		fprintf(stderr, fmt, ##args); \
	} while(0) \

extern "C" {
	void __cyg_profile_func_enter(void* callee, void* callsite)
	__attribute__((no_instrument_function));
	void __cyg_profile_func_enter(void* callee, void* callsite)
	{
		debug_print("Entering %p in %pn", callee, callsite);
	}
	void __cyg_profile_func_exit(void* callee, void* callsite)
	__attribute__((no_instrument_function));
	void __cyg_profile_func_exit(void* callee, void* callsite) {
		debug_print("Exiting %p in %pn", callee, callsite);
	}
	void foo() {
		printf("foo()\n");
	}
}

int main() {
	foo();
	return 0;
}
```
编译运行：

```
$ g++ t.cc -finstrument-functions -g
$ ./a.out
Entering 0x8048690 in 0xb764ec76
Entering 0x804862a in 0x80486b3
foo()
Exiting 0x804862a in 0x80486b3
Exiting 0x8048690 in 0xb764ec76
```
这里输出的都是函数地址信息,如果想定位到函数代码,可以借助 addr2line 工具:

```
$ addr2line -e a.out -f -s 0x804862a 0x80486b3
foo
t.cc:22
main
t.cc:29
```
最后回到 `__attribute__((no_instrument_function))`, 这个选项的作用就是用来禁止编译器向指定的函数内部添加`__cyg_profile_func_enter()` 和 `__cyg_profile_func_exit()`调用代码,例如用户可能会在`__cyg_profile_func_enter` 函数中调用自定义的函数, 这些被调用的自定义函数声明中都要加上`__attribute__((no_instrument_function))`属性, 避免产生无限递归的调用. 另外也可以通过 GCC 的以下两个选项来实现同样的作用:

```
-finstrument-functions-exclude-file-list=file1,file2,... #屏蔽file1,file2 中的函数
-finstrument-functions-exclude-function-list=func1,func2,... #屏蔽func1,func2 函数
```

另外可以参考一篇 developerWorks 的文章:[用 Graphviz 可视化函数调用](https://www.ibm.com/developerworks/cn/linux/l-graphvis/)

## 2.6 deprecated
设置了这一属性的函数/变量/类型在代码中被使用的时候会使编译器产生一条警告, 例如

```c
void foo() __attribute__((deprecated));
void foo(){}
int main() {
	foo();
	return 0;
}
```

编译:

```
$ gcc test.c
test.c: In function ‘main’:
test.c:6: warning: ‘foo’ is deprecated (declared at test.c:3)
```
这个选项可以用来在检验代码升级的时候旧版本的代码**是否都已经被移除**.