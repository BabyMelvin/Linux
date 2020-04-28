#!/bin/bash

##########
# 子shell变量对父shell不可见，即使是export为环境变量，父shell仍不可见子shell
# 子shell能够从父shell继承如下属性:
#     1.当前工作目录
#     2.环境变量
#     3.标准输入、标准输出和标准错误输出
#     4.多有已打开的文件标识
#     5.忽略的信号
# 子shell不能从父shell继承得来的属性
#     1. 除了环境变量和.bashrc文件中定义的变量之外的shell变量
#     2. 未被忽略的信号处理
#
# 子shell能设置独立于父shell子环境
##########

set -C # 开启-C选项，防止重定向时覆盖文件
# 在父shell定义outerval
echo "---------------------IN MAINSHELL-----------------"
outerval=OUTER
echo "outerval=$outerval"

( #进入子shell
echo "---------------------IN SUBSHELL------------------"
innterval=INNER
echo "innterval=$innterval"
outerval=OUTER-INNER    # 更改父shell所定义的outerval变量值
echo "outerval=$outerval"

# 将innershell 和 outerval export
export innterval
export outerval
echo "---------------------OUT SUBSHELL------------------"
)
echo "---------------------IN MAINSHELL-----------------"

echo "innterval=$innterval"
echo "outerval=$outerval"

