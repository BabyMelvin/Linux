## 变量

使用`${}`进行变量的引用，在IF等语句中，直接使用变量而不是通过`${}`值.

### 1.自定义变量

```cmake
set(HELLO_SRC main.c SOURCE_PATH)
```
将设置变量`HELLO_SRC`的值为`main.c SOURCE_PATH`.

### 2.cmake中常用的常量

* CMAKE_BINARY_DIR、PROJECT_BINARY_DIR、_BINARY_DIR这三个变量指代的内容是一致的
	* 如果是`in source `编译，指得就是工程顶层目录。
	* 如果是`out-of-source`编译，指的是工程编译发生的目录。
* `CMAKE_SOURCE_DIR`  `PROJECT_SOURCE_DIR`  `_SOURCE_DIR`这三个变量指代的内容是一致的.
	* 不论采用何种编译方式，都是工程顶层目录。
* `CMAKE_CURRENT_SOURCE_DIR` 指的是当前处理的`CMakeLists.txt` 所在的路径，比如上面我们提到的src子目录。 
* `CMAKE_CURRRENT_BINARY_DIR`如果是in-source 编译，它跟`CMAKE_CURRENT_SOURCE_DIR `一致，如果是out-of-source 编译，他指的是target 编译目录。
* `CMAKE_CURRENT_LIST_FILE` 输出调用这个变量的CMakeLists.txt 的完整路径
* `CMAKE_CURRENT_LIST_LINE` 输出这个变量所在的行
* `CMAKE_MODULE_PATH`这个变量用来定义自己的cmake 模块所在的路径。如果你的工程比较复杂，有可能会自己编写一些cmake 模块，这些cmake 模块是随你的工程发布的，为了让cmake 在处理CMakeLists.txt 时找到这些模块，你需要通过SET指令，将自己的cmake 模块路径设置一下。`SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)` 这时候你就可以通过`INCLUDE 指令`来调用自己的模块了。
* `EXECUTABLE_OUTPUT_PATH` 和`LIBRARY_OUTPUT_PATH `分别用来重新定义最终结果的存放目录，前面我们已经提到了这两个变量。
* `PROJECT_NAME` 返回通过`PROJECT` 指令定义的项目名称。

### 3.cmake 调用环境变量的方式

使用`$ENV{NAME}` 指令就可以调用系统的环境变量了。比如`MESSAGE(STATUS "HOME dir: $ENV{HOME}")`,设置环境变量的方式是：`SET(ENV{ 变量名} 值)`
* 1,`CMAKE_INCLUDE_CURRENT_DIR`

自动添加`CMAKE_CURRENT_BINARY_DIR` 和`CMAKE_CURRENT_SOURCE_DIR` 到当前处理的`CMakeLists.txt` 。相当于在每个`CMakeLists.txt` 加入：
`INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}${CMAKE_CURRENT_SOURCE_DIR})`

* 2.`CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE`

### 4.主要的开关选项

* 1，`CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS` ，用来控制`IF ELSE` 语句的书写方式，在下一节语法部分会讲到。

* 2，`BUILD_SHARED_LIBS`

这个开关用来控制`默认的库编译方式`，如果不进行设置，使用`ADD_LIBRARY` 并没有指定库类型的情况下，默认编译生成的库都是`静态库`。如果`SET(BUILD_SHARED_LIBS ON)` 后，默认生成的为动态库。

* ３，`CMAKE_C_FLAGS` 设置`C编译选项`，也可以通过指令`ADD_DEFINITIONS()` 添加。

* 4，`CMAKE_CXX_FLAGS` 设置`C++ 编译选项`，也可以通过指令`ADD_DEFINITIONS()` 添加。

## cmake常用指令

### 1.基本指令

* 1.`ADD_DEFINITIONS`向C/C++编译器添加`-D`定义,比如:`ADD_DEFINITIONS(-DENABLE_DEBUG -DABC)`，参数之间用空格分割。如果你的代码中定义了：`#ifdef ENABLE_DEBUG #endif`这个代码块就会生效。如果要添加其他的编译器开关，可以通过`CMAKE_C_FLAGS` 变量和`CMAKE_CXX_FLAGS` 变量设置。

* 2.`ADD_DEPENDENCIES`定义target 依赖的其他target ，确保在编译本target 之前，其他的target 已经被构建。`ADD_DEPENDENCIES(target-name depend-target1depend-target2 ...)`
* ３，`ADD_EXECUTABLE` 、`ADD_LIBRARY`、`ADD_SUBDIRECTORY `前面已经介绍过了，这里不再罗唆。
* ４，`ADD_TEST` 与`ENABLE_TESTING` 指令。ENABLE_TESTING 指令用来控制Makefile 是否构建test目标，涉及工程所有目录。语法很简单，没有任何参数，ENABLE_TESTING() ，一般情况这个指令放在工程的主CMakeLists.txt 中.
* ５，`AUX_SOURCE_DIRECTORY`基本语法是：`AUX_SOURCE_DIRECTORY(dir VARIABLE)`作用是发现一个目录下所有的源代码文件并将列表存储在一个变量中，这个指令临时被用来自动构建源文件列表。因为目前cmake 还不能自动发现新添加的源文件。你也可以通过后面提到的`FOREACH 指令`来处理这个`LIST`

```cmake
AUX_SOURCE_DIRECTORY(. SRC_LIST)
ADD_EXECUTABLE(main ${SRC_LIST})
```
* 6.`CMAKE_MINIMUM_REQUIRED`其语法为`CMAKE_MINIMUM_REQUIRED(VERSION versionNumber [FATAL_ERROR])`比如`CMAKE_MINIMUM_REQUIRED(VERSION 2.5 FATAL_ERROR)`如果cmake 版本小与2.5，则出现严重错误，整个过程中止。
* 7，`EXEC_PROGRAM`在`CMakeLists.txt` 处理过程中执行命令，并不会在生成的Makefile 中执行。具体语法为：`EXEC_PROGRAM(Executable [directory in which to run][ARGS ][OUTPUT_VARIABLE ][RETURN_VALUE ])`用于在指定的目录运行某个程序，通过ARGS 添加参数，如果要获取输出和返回值，可通过
`OUTPUT_VARIABLE` 和`RETURN_VALUE` 分别定义两个变量. 这个指令可以帮助你在CMakeLists.txt 处理过程中支持任何命令，比如根据系统情况去修改代码文件等等。举个简单的例子，我们要在src目录执行ls命令，并把结果和返回值存下来。cmake实践
目录搜索
版权
内容
一、前言
二、安装cmake
三、初试cmake
四、更好一点的Hello World
五、静态库与动态库构建
六、如何使用外部共享库和头文件
七、cmake 常用变量和常用环境变量
八、cmake 常用指令
九、复杂的例子
本文档使用 看云 构建
#八、cmake 常用指令

前面我们讲到了cmake 常用的变量，相信“cmake 即编程的感觉会越来越明显，无论如何，我们仍然可以看到cmake 比autotools要简单很多。接下来我们就要集中的看一看cmake 所提供的常用指令。在前面的章节我们已经讨论了很多指令的用法，如PROJECT，ADD_EXECUTABLE，INSTALL，ADD_SUBDIRECTORY，SUBDIRS，INCLUDE_DIRECTORIES，LINK_DIRECTORIES，TARGET_LINK_LIBRARIES，SET等。

本节会引入更多的cmake指令，为了编写的方便，我们将按照cmake man page的顺序来介绍各种指令，不再推荐使用的指令将不再介绍，INSTALL系列指令在安装部分已经做了非常详细的说明，本节也不在提及。(你可以将本章理解成选择性翻译，但是会加入更多的个人理解)

##一，基本指令

1，ADD_DEFINITIONS 向C/C++ 编译器添加-D定义，比如: ADD_DEFINITIONS(-DENABLE_DEBUG -DABC)，参数之间用空格分割。

如果你的代码中定义了：#ifdef ENABLE_DEBUG #endif这个代码块就会生效。如果要添加其他的编译器开关，可以通过CMAKE_C_FLAGS 变量和CMAKE_CXX_FLAGS 变量设置。

２，ADD_DEPENDENCIES

定义target 依赖的其他target ，确保在编译本target 之前，其他的target 已经被构建。

ADD_DEPENDENCIES(target-name depend-target1depend-target2 ...)

３，ADD_EXECUTABLE 、ADD_LIBRARY 、ADD_SUBDIRECTORY 前面已经介绍过了，这里不再罗唆。

４，ADD_TEST 与ENABLE_TESTING 指令。

ENABLE_TESTING 指令用来控制Makefile 是否构建test目标，涉及工程所有目录。语法很简单，没有任何参数，ENABLE_TESTING() ，一般情况这个指令放在工程的主CMakeLists.txt 中.

ADD_TEST 指令的语法是:

ADD_TEST(testname Exename arg1 arg2 ...)

testname 是自定义的test 名称，Exename 可以是构建的目标文件也可以是外部脚本等等。后面连接传递给可执行文件的参数。如果没有在同一个CMakeLists.txt 中打开ENABLE_TESTING() 指令，任何ADD_TEST 都是无效的。

比如我们前面的Helloworld 例子，可以在工程主CMakeLists.txt 中添加ADD_TEST(mytest ${PROJECT_BINARY_DIR}/bin/main) ENABLE_TESTING()

生成Makefile 后，就可以运行make test 来执行测试了。

５，AUX_SOURCE_DIRECTORY 基本语法是：

AUX_SOURCE_DIRECTORY(dir VARIABLE)

作用是发现一个目录下所有的源代码文件并将列表存储在一个变量中，这个指令临时被用来自动构建源文件列表。因为目前cmake 还不能自动发现新添加的源文件。

比如

AUX_SOURCE_DIRECTORY(. SRC_LIST)ADD_EXECUTABLE(main ${SRC_LIST})

你也可以通过后面提到的FOREACH 指令来处理这个LIST

6，CMAKE_MINIMUM_REQUIRED其语法为CMAKE_MINIMUM_REQUIRED(VERSION versionNumber [FATAL_ERROR])比如CMAKE_MINIMUM_REQUIRED(VERSION 2.5 FATAL_ERROR)如果cmake 版本小与2.5，则出现严重错误，整个过程中止。

7，EXEC_PROGRAM

在CMakeLists.txt 处理过程中执行命令，并不会在生成的Makefile 中执行。具体语法为：

`EXEC_PROGRAM(Executable [directory in which to run][ARGS ][OUTPUT_VARIABLE ][RETURN_VALUE ])`

用于在指定的目录运行某个程序，通过ARGS 添加参数，如果要获取输出和返回值，可通过

`OUTPUT_VARIABLE` 和`RETURN_VALUE` 分别定义两个变量. 这个指令可以帮助你在CMakeLists.txt 处理过程中支持任何命令，比如根据系统情况去修改代码文件等等。

举个简单的例子，我们要在src目录执行ls命令，并把结果和返回值存下来。

可以直接在`src/CMakeLists.txt` 中添加：

```cmake
EXEC_PROGRAM(
			ls ARGS "*.c" OUTPUT_VARIABLE LS_OUTPUT RETURN_VALUELS_RVALUE)
			IF(not LS_RVALUE)
				MESSAGE(STATUS "ls result: " ${LS_OUTPUT})
			ENDIF(not LS_RVALUE)
```
在cmake 生成Makefile的过程中，就会执行ls命令，如果返回0，则说明成功执行，那么就输出ls *.c 的结果。关于IF语句，后面的控制指令会提到。

* 8，`FILE指令`文件操作指令，基本语法为:
```cmake
FILE(WRITE filename "message to write"... )
FILE(APPEND filename "message to write"... ) 
FILE(READ filename variable) 
FILE(GLOB variable [RELATIVE path][globbing expressions]...)
FILE(GLOB_RECURSE variable [RELATIVE path] [globbing expressions]...)
FILE(REMOVE [directory]...)
FILE(REMOVE_RECURSE [directory]...)
FILE(MAKE_DIRECTORY [directory]...)
FILE(RELATIVE_PATH variable directory file)
FILE(TO_CMAKE_PATH path result)FILE(TO_NATIVE_PATH path result)
``` 
这里的语法都比较简单，不在展开介绍了。

* 9，`INCLUDE 指令`，用来载入`CMakeLists.txt` 文件，也用于载入预定义的`cmake 模块`. `INCLUDE(file1 [OPTIONAL]) INCLUDE(module [OPTIONAL])` `OPTIONAL 参数`的作用是文件不存在也不会产生错误。你可以指定载入一个文件，如果定义的是一个模块，那么将在`CMAKE_MODULE_PATH `中搜索这个模块并载入。载入的内容将在处理到`INCLUDE 语句`是直接执行。

### 2.INSTALL 指令
INSTALL 系列指令已经在前面的章节有非常详细的说明，这里不在赘述，可参考前面的安装部分。

### 3.FIND_ 指令
FIND_ 系列指令主要包含一下指令：
```cmake
FIND_FILE( name1 path1 path2 ...) VAR 变量代表找到的文件全路径，包含文件名
FIND_LIBRARY( name1 path1 path2 ...) VAR 变量表示找到的库全路径，包含库文件名
FIND_PATH( name1 path1 path2 ...) VAR 变量代表包含这个文件的路径。
FIND_PROGRAM( name1 path1 path2 ...) VAR 变量代表包含这个程序的全路径
FIND_PACKAGE(<name> [major.minor] [QUIET] [NO_MODULE][[REQUIRED|COMPONENTS] [componets...]])
```
用来调用预定义在`CMAKE_MODULE_PATH `下的`Find.cmake` 模块，你也可以自己定义Find 模块，通过`SET(CMAKE_MODULE_PATH dir)` 将其放入工程的某个目录中供工程使用，我们在后面的章节会详细介绍FIND_PACKAGE 的使用方法和Find 模块的编写。FIND_LIBRARY 示例：

```cmake
FIND_LIBRARY(libX X11 /usr/lib)
IF(NOT libX)
MESSAGE(FATAL_ERROR “libX not found”)
ENDIF(NOT libX)
```

## 4 控制指令

* 1,IF 指令，基本语法为：

```cmake
IF(expression)# THEN section.COMMAND1(ARGS ...)

COMMAND2(ARGS ...)
...

ELSE(expression)# ELSE section.COMMAND1(ARGS ...)COMMAND2(ARGS ...)...

ENDIF(expression)
```
另外一个指令是`ELSEIF`，总体把握一个原则，凡是出现IF的地方一定要有对应的ENDIF. 出现ELSEIF 的地方，ENDIF 是可选的。

表达式的使用方法如下:

* `IF(var)` ，如果变量不是：空，0，N, NO, OFF, FALSE, NOTFOUND 或_NOTFOUND 时，表达式为真。
* `IF(NOT var )`，与上述条件相反。
* `IF(var1 AND var2)` ，当两个变量都为真是为真。
* `IF(var1 OR var2)` ，当两个变量其中一个为真时为真。
* `IF(COMMAND cmd)` ，当给定的cmd 确实是命令并可以调用是为真。
* `IF(EXISTS dir)` 或者`IF(EXISTS file)` ，当目录名或者文件名存在时为真。
* `IF(file1 IS_NEWER_THAN file2)` ，当file1 比file2 新，或者file1/file2 其中有一个不存在时为真，文件名请使用完整路径。
* `IF(IS_DIRECTORY dirname)` ，当dirname 是目录时，为真。
* `IF(variable MATCHES regex)` `IF(string MATCHES regex)`当给定的变量或者字符串能够匹配正则表达式`regex` 时为真。比如：

```cmake
IF("hello" MATCHES "ell")MESSAGE("true")ENDIF("hello" MATCHES "ell")
IF(variable LESS number)
IF(string LESS number)IF(variable GREATER number)IF(string GREATER number)
IF(variable EQUAL number)IF(string EQUAL number)
```

* 数学表达式

```cmake
IF(variable STRLESS string)
IF(string STRLESS string)
IF(variable STRGREATER string)
IF(string STRGREATER string)
IF(variable STREQUAL string
)IF(string STREQUAL string)
```
按照字母序的排列进行比较.`IF(DEFINED variable)` ，如果变量被定义，为真。一个小例子，用来判断平台差异：

```cmake
IF(WIN32) MESSAGE(STATUS “This is windows.”) #作一些Windows 相关的操作
ELSE(WIN32) MESSAGE(STATUS “This is not windows”) #作一些非Windows 相关的操作
ENDIF(WIN32)
```
上述代码用来控制在不同的平台进行不同的控制，但是，阅读起来却并不是那么舒服，ELSE(WIN32) 之类的语句很容易引起歧义。这就用到了我们在常用变量一节提到的`CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS`开关。可以`SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)`这时候就可以写成:`IF(WIN32)ELSE()ENDIF()`如果配合ELSEIF 使用，可能的写法是这样:

```cmake
IF(WIN32)#do something related to WIN32ELSEIF(UNIX)#do something related to UNIX
ELSEIF(APPLE)#do something related to APPLE
ENDIF(WIN32)
```

* 2,`WHILEWHILE` 指令

```cmake
WHILE(condition)COMMAND1(ARGS ...)COMMAND2(ARGS ...)...
ENDWHILE(condition) 
```
其真假判断条件可以参考IF指令。

* 3,`FOREACH`
 FOREACH 指令的使用方法有三种形式：

* 1，列表

```cmake
FOREACH(loop_var arg1 arg2 ...)COMMAND1(ARGS ...)COMMAND2(ARGS ...)...
ENDFOREACH(loop_var) 
```
像我们前面使用的`AUX_SOURCE_DIRECTORY` 的例子

```cmake
AUX_SOURCE_DIRECTORY(. SRC_LIST) FOREACH(F ${SRC_LIST})
MESSAGE(${F})ENDFOREACH(F)
```
2，范围

`FOREACH(loop_var RANGE total)ENDFOREACH(loop_var)`从0到total 以１为步进.举例如下：

```cmake
FOREACH(VAR RANGE 10)MESSAGE(${VAR})ENDFOREACH(VAR)
```
最终得到的输出是：

```cmake
0
1
2
3
4
5
6
7
8
9
10
```

３，范围和步进

`FOREACH(loop_var RANGE start stop [step])ENDFOREACH(loop_var)`从start 开始到stop结束，以step为步进，举例如下:

```cmake
FOREACH(A RANGE 5 15 3)MESSAGE(${A})ENDFOREACH(A)
```
最终得到的结果是：

```
5
8
11
14
```
这个指令需要注意的是，知道遇到ENDFOREACH 指令，整个语句块才会得到真正的执行。

**特别需要注意的是**，在控制语句条件中使用变量，不能用${} 引用，而是直接应用变量名。