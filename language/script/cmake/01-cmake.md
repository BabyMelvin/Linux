# CMAKEList学习
## 1.可执行程序
CMakeLists.txt文件内容与main.cc同目录
```
#CMake 最低版本要求
cmake_minimum_required (VERISION 2.8)

# 项目信息
project (Demo1)

# 指定生成目标
add_executable(Demo main.cc)
```
**编译**：当前目录执行 `cmake .`,得到Makefile后再使用`make`命令编译得到Demo可执行文件。
## 2.多个源文件
### 同一个目录，多个源文件
```
# CMake 最低版本要求
cmake_minimum_required (vERSION 2.8)

# 项目信息
project(Demo2)

#指定生成目标
add_executable(Demo main.cc MathFunctions.cc)
```
把所有源文件添加进去简便写法`aux_source_directory(<dir> <variable>)`,查找指定目录下所有源文件。
改成CMakeList.txt：
```
#CMake 最低版本要求
cmake_minmum_required (VERSION 2.8)

#项目信息
project (Demo2)
#查找当前目录下所有源文件,并将名称保存在DIR_SRCS变量
aux_source_directory(. DIR_SRCS)

# 指定生成目标
add_executable(Demo ${DIR_SRCS})
```
CMake会将所有源文件的文件名赋值给变量DIR_SRCS,再指示变量DIR_SRCS中源文件需要编译成一个名称为Dem可执行文件.
### 多个目录，多个源文件
目录结构./main.cc,math/MathFunctions.cc,
这种情况需要分别在项目根目录.和math目录中个编写一个CMakeLists.txt文件。方便可将math目录编译成静态库再由main
函数调用.

根目录的CMakeLists.txt:
```
# CMake 最低版本号要求
cmake_minimum_required （VERSION 2.8）

# 项目信息
project (Demo3)

# 查找当前目录下所有的原文件，并将名称保存到DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 添加 math子目录
add_subdirectory(math)
 # 指定生成目录
 add_executable(Demo main.cc)
 
 # 添加链接表
 target_link_libraries(Demo MathFunctions)
```
其中`add_subdirectory`指明项目包含一个子目录math，这样math目录CMakeLists.txt文件和源码也会被处理。
命令`target_link_libraries`指明可执行文件main需要一个名为MathFuncitons的链接库

子目录中的CMakeLists.txt文件:
```
# 查找当前目录下的所有文件，并保存名称到DIR_LIB_SRC 变量
aux_source_directory(. DIR_LIB_SRCS)

# 生成链接库
add_library (MathFunctions ${DIR_LIB_SRCS})
```
在该文件中使用命令`add_library`将src目录中的源文件编译为静态链接库.
## 3.自定义编译选项
> CMAKE允许项目增加编译选项，从而可以根据用户环境和需求选择最适合的编译方案。

如：可将MathFunctions库设为一个可选的库，如果该选项为ON，就使用该库的数学函数，否则调用标准库的数学函数库。
修改CMakeLists文件。

```
# CMake 最低版号要求
cmake_minimum_required　（VERSION 2.8)

# 项目信息
project (Demo4)

#　加入一个配置文件，用于处理CMake对源码设置
#  whitespace not ","
configure_file(
"${PROJECT_SOURCE_DIR}/config.h.in"
"${PROJECT_BINARY_DIR}/config.h"
)
# 是否使用自己的MathFunctions库
option (USE_MYMATH "use provided math implementation" ON)

# 是否加入MathFunctions库
if(USE_MYMATH)
 include_directories ("${PROJECT_SOURCE_DIR}/math")
 add_subdirectory (math)
 set (EXTRA_LIBS ${EXTRA_LIBS} MathFunctions)
endif (USE_MYMATH)

# 查找当前目录下所有源文件，并将保存到DIR_SRCS变量
aux_source_directory (. DIR_SRCS)

#　指定生成目标
add_executable(Demo ${DIR_SRCS})
target_link_libraries (Demo ${EXTRA_LIBS})
```
其中`configure_file`命令用于加入一个配置头文件config.h,这个文件有CMake从config.h.in生成，
通过这样的机制，将可通过预定义一些参数和变量控制代码的生成．

命令option添加一个`USE_MYMATH`选项，并默认值为`ON`.(if config.h.in not config?)

变量值USE_MYMATH决定是否使用我们自己编写MathFunctions库

修改main.cc文件
```
#include
#include
#include "config.h"
#ifdef USE_MYMATH
 #include "math/MathFunctions.h"
#else
 #include
#endif
int main(int argc,char*argv[]){
  double base=atof(argv[1]);
  int exponent=atoi(argv[2]);
  #ifdef USE_MYMATH
   printf("Now we use our own Math library.\n");
   double result=power(base,exponet);
  #else
   printf("Now we use the standard library.\n");
   double result=pow(base,exponent);
  #endif
  printf("%g ^ %d is %g\n", base, exponent, result);
  return 0;
}
```

编写config.h.in文件

不直接编写config.h文件，这个文件预定义了USE_MYMATH的值。方便CMakeLists.txt中导入配置，编写一个
config.h.in文件。内容:
```
//mean off USE_MYMATH
//open to use
//#cmakedefine USE_MYMATH ON
#cmakedefine USE_MYMATH
```
CMake会自动根据CMakeLists配置文件中设置自动生成config.h文件

编译项目

可以使用ccmake配置选项。`方向键`跳转，`enter`修改，`c`完成配置,`g`确认生成Makefile.

USE_MYMATH 为 ON

此时config.h的内容为:
```
#define USE_MYMATH
```
USE_MYMATH 为 OFF
此时config.h内容为：
```
/*#undef USE_MYMATH*/
```

## 4.安装和测试
> CMake可以指定安装规则，以及添加测试。

### 定制安装规则
在 math/CMakeLists.txt文件添加：
```
#指定 MathFunctions库安装路径
install (TARGETS MathFunctions DESTIANTION lib)
install (FILES MathFunctions.h DESTIANTION include)
```
指明MathFunctions库安装路径。之后同样修改目录CMakeLists文件，添加如下:
```
#指定安装路径
install (TARGETS Demo DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/config.h"
         DESTINATION include)
```
通过上面的定制，生成Demo文件和MathFunctions函数库libMathFunctions.o文件会被复制到`/usr/local/bin`中,
而MathFunctions.h和生成的config.h文件则会被复制到`/usr/local/include`中。
运行验证：
```
[ehome@xman Demo5]$ sudo make install
[ 50%] Built target MathFunctions
[100%] Built target Demo
Install the project...
-- Install configuration: ""
-- Installing: /usr/local/bin/Demo
-- Installing: /usr/local/include/config.h
-- Installing: /usr/local/lib/libMathFunctions.a
-- Up-to-date: /usr/local/include/MathFunctions.h
[ehome@xman Demo5]$ ls /usr/local/bin
Demo  libMathFunctions.a
[ehome@xman Demo5]$ ls /usr/local/include
config.h  MathFunctions.h
```

### 为工程添加测试
CMake提供一个称为CTest的测试工具.我们要做的只是在项目根目录CMakeLists.txt文件中调用一系列`add_test`命令。
```
#启用测试
enable_testing()

#测试程序是否成功运行
add_test (test_run Demo 5 2)

#测试帮助信息是否可以正常提示
add_test(test_usage Demo)
set_tests_properties (test_usage
                    PROPERTTIES PASS_REGULAR_EXPRESSION "Usage: .* base exponent")

#测试 5 的平方
add_test (test_5_2 Demo 5 2)
set_tests_properties(test_5_2
                    PROPERTIES PASS_REGULAR_EXPRESSION "is 25")

#测试 10的5次方
add_test (test_10_5 Demo 10 5)
set_tests_properties(test_10_5
         PROPERTIES PASS_REGULAR_EXPRESSION "is 100000")
 
# 测试2的10次方
add_test (test_2_10 Demo 2 10)
set_tests_properties(test_2_10
         PROPERTIES PASS_REGULAR_EXPRESSION "is 1024")
```
上面的代码包含了四个测试。第一个测试 `test_run `用来测试程序是否成功运行并返回 0 值。剩下的三个测试分别用来测试 5 的 平方、10 的 5 次方、2 的 10 次方是否都能得到正确的结果。其中 `PASS_REGULAR_EXPRESSION` 用来测试输出是否包含后面跟着的字符串。

让我们看看测试的结果：

```
[ehome@xman Demo5]$ make test
Running tests...
Test project /home/ehome/Documents/programming/C/power/Demo5
    Start 1: test_run
1/4 Test #1: test_run .........................   Passed    0.00 sec
    Start 2: test_5_2
2/4 Test #2: test_5_2 .........................   Passed    0.00 sec
    Start 3: test_10_5
3/4 Test #3: test_10_5 ........................   Passed    0.00 sec
    Start 4: test_2_10
4/4 Test #4: test_2_10 ........................   Passed    0.00 sec
100% tests passed, 0 tests failed out of 4
Total Test time (real) =   0.01 sec
```
如果要测试更多输入数据，可以编写成宏实现：
```
#定义一个宏，用来简化测试工作
macro (do_test arg1 arg2 result)
  add_test(test_${arg1}_${arg2} Demo ${arg1} $arg2})
  set_tests_properties (test_${arg1}_$arg2}
           PROPERTIES PASS_REGULAR_EXPRESSION ${result})
endmacro (do_test)

# 使用该宏进行一系列的数据测试
do_test  (5 2 "is 25")
do_tesst (10 5 "is 100000")
do_test  (2 10 "is 1024")
```
关于 CTest 的更详细的用法可以通过 man 1 ctest 参考 CTest 的文档。

## 5.支持gdb

让CMake支持gdb设置很容易，只需要指定Debug模式下开启 -g选项:
```
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS) -O3 -Wall")
```

## 6.添加环境检查
> 有时候可能对系统环境做点检查

例如有时候对系统检查是否带pow函数。如果有使用，否则使用自己的power函数。
### 添加CheckFunctionExists宏
首先在顶层CMakeLists文件中添加CheckFunctionExists.cmake宏，并调用`check_function_exists`命令,
测试链接器是否能够在链接阶段找到pwd函数。
```
# 检查系统是否支持pow函数
include (${CMAKE_ROOT}/Moudles/CheckFunctionExists.cmake)
check_function_exists (pow HAVE_POW)
```
将上面放在`configure_file`命令前。

### 预定义相关宏变量
预定义修改config.h.in文件，预定义相关的宏变量
```
//Does the platform provide pow funciton?
#cmakedefine HAVE_POW
```
### 在代码中使用宏和函数
最后一步是修改main.cc，在代码中使用宏和函数：
```
#ifdef HAVA_POW
  printf("Now we use the standard library. \n");
  double result = pow(base, exponent);
#else
   printf("Now we use our own Math library. \n");
   double result = power(base, exponent);
#endif
```

## 7.添加版本号

顶层CMakeLists文件，在`project`命令之后加入两行:
```
set (Demo_VERSION_MAJOR 1)
set (Demo_VERSION_MINOR 0)
```
之后为了代码获取版本信息，可以修改config.h.in文件，添加两个预定义变量:
```
// the configured options and settings for Tutorial
#define Demo_VERSION_MAJOR @Demo_VERSION_MAJOR@
#define Demo_VERSION_MINOR @Demo_VERSION_MINOR@
```
在代码中使用，打印版本信息：

```
#include <stdio.h>

int main(int argc,char *argv[]){
   printf("%s Version %d.%d\n",
            argv[0],
            Demo_VERSION_MAJOR,
            Demo_VERSION_MINOR);
        printf("Usage: %s base exponent \n", argv[0]);
        return 1;
}
```


























