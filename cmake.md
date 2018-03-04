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
