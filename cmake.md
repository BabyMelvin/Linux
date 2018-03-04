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
configure_file(
"${PROJECT_SOURCE_DIR}/config.h.in",
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

命令option添加一个`USE_MYMATH`选项，并默认值为`ON`.

变量值USE_MYMATH决定是否使用我们自己编写MathFunctions库


















































