#!/bin/bash

# 1.读取参数
factorial=1
for (( number = 1; number <= $1; number++))
do
	factorial=$[ $factorial * $number ]
done
echo The factor of $1 is $factorial

total=$[ ${10} * ${11} ]
echo The tenth parameter is ${10}
echo The eleventh parameter is ${11}
echo The total is $total

# 2.显示程序名
name=`basename $0`
echo "the command entered is $name"

# 3.获取参数的个数
# $#获取参数的个数，./test 1 2 3 4 ,则$#=4
echo There were $# parameters supplied

# 获取最后一个参数 
param=$#

echo "the last parameter is $param"
echo "the last parameter is ${!#}"

# 4.获取所有输入参数
# $*会把所有参数当做一个单词处理
# $@会把所有参数当做一个字符串处理,可以遍历所有参数

count=1
for param in "$*"
do
    echo "\$* parameter #$count = $param"
    count=$[ $count + 1]
done

count=1
for param in "$@"
do
    echo "\$@ parameter #$count = $param"
    count=$[ $count + 1]
done

