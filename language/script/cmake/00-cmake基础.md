### 初试cmake

* 1.`main.c`文件内容

```c
#include <stdio.h>
int main(){
	printf("hello world from t1\n");
	return 0;
}
```

同目录下的CMakeLists.txt文件:

```cmake
PROJECT(HELLO)
SET(SRC_LIST main.c)
MESSAGE(STATUS "this is BINARY dir" ${HELLO_BINAY_DIR})
MESSAGE(STATUS "this is source dir"  ${HELLO_SROUCE_DIR})

ADD_EXECUTABLE(hello ${SRC_LIST})
```

**构建多有的文件**

* 1.`cmake .`

再让我们看一下目录中的内容：你会发现，系统自动生成了：`CMakeFiles`,`CMakeCache.txt`, `cmake_install.cmake` 等文件，并且生成了`Makefile`.

避免很多文件和源文件同目录，新建out目录,执行下面命令之后产生的文件将会在out目录中：

```
cd out
cmake ..
```

when `cmake ..`:

* `PROJECT_SOURCE_DIR`:still with the same with CMakeLists.txt file
* `PROJECT_BINARY_DIR`:watch out ,which is the cmake execute location,such as "out"

* 2.`make`命令

大概会得到如下的彩色输出：

```cmake
Scanning dependencies of target hello [100%] Building C object CMakeFiles/hello.dir/main.o
 Linking C executable hello [100%] Built target hello
```
如果你需要看到make 构建的详细过程，可以使用`make VERBOSE=1` 或者`VERBOSE=1 ` make 命令来进行构建。
* 3.运行 `./hello`
* 4.清理工程：`make clean`

### 语法简单解释

* `PROJECT` 指令的语法是：`PROJECT(projectname [CXX] [C] [Java])`你可以用这个指令定义工程名称，并可指定工程支持的语言，支持的语言列表是可以忽略的，默认情况表示支持所有语言。这个指令隐式的定义了两个cmake 变量: `_BINARY_DIR` 以及`_SOURCE_DIR` ，这里就是`HELLO_BINARY_DIR` 和`HELLO_SOURCE_DIR`( 所以CMakeLists.txt 中两个MESSAGE 指令可以直接使用了这两个变量)，因为采用的是内部编译，两个变量目前指的都是**工程所在路径**，后面我们会讲到外部编译，两者所指代的内容会有所不同。
    * 同时cmake 系统也帮助我们预定义了`PROJECT_BINARY_DIR` 和`PROJECT_SOURCE_DIR` 变量，他们的值分别跟`HELLO_BINARY_DIR` 与`HELLO_SOURCE_DIR` 一致。
* SET 指令的语法是：`SET(VAR [VALUE] [CACHE TYPE DOCSTRING[FORCE]])`
* MESSAGE 指令的语法是:

```
MESSAGE([SEND_ERROR | STATUS | FATAL_ERROR] "message to display"
...)
```
包含了三种类型:

* SEND_ERROR,产生错误,生成过程被跳过。 
* SATUS,输出前缀为—的信息* 
* FATAL_ERROR,立即终止所有 cmake 过程。

* `ADD_EXECUTABLE(hello ${SRC_LIST})`定义了这个工程会生成一个文件名为hello的可执行文件，相关的源文件是SRC_LIST 中定义的源文件列表，本例中你也可以直接写成`ADD_EXECUTABLE(hello main.c)` 。

### 内部构建与外部构建

* 上面的例子展示的是“内部构建”生成的临时文件比您的代码文件还要多的时候
* 举个简单的例子来说明外部构建，以编译wxGTK动态库和静态库为例，在Everest 中打包方式是这样的：解开wxGTK 后。在其中建立static 和shared 目录。
	* 进入`static`目录，运行`../configure –enable-static;make `会在`static 目录`生成`wxGTK 的静态库`。
	* 进入`shared`目录，运行`../configure –enable-shared;make `就会在`shared 目录`生成动态库。
