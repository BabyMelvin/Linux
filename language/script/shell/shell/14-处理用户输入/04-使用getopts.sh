#!/bin/bash

# getopts命令会产生两个变量:OPTIND和OPTARG
# OPTIND初始值为1，表示下一个待处理的参数的索引，只要存在，getopts命令返回true,所以一般命令使用while循环
# OPTARG:getopts获取其期望的参数后存入的位置。如果不在期望内，则$opt被设为?并将该意外值存入OPTARG
#    如果$opt需要拥有具体设置而实际没有，则$opt被设为:并将丢弃设置的opt存入OPTARG

# 1.基本使用
while getopts :ab:c opt
do
    echo "$opt"
	case "$opt" in
	a) echo "Found the -a option";;
	b) echo "Found the -b option, with value $OPTARG";;
	c) echo "Found the -c option";;
	*) echo "Unknown option:$opt";;
	esac
done

# 不使用shift和破折号
# 可以使用空格和双引号：./test -b "test1 test2" -a 或 ./test -abtest1,未定义的统一输出为?

# 2.OPTIND使用
while getopts :ab:cd opt
do
	case "$opt" in
	a) echo "Found the -a option";;
	b) echo "Found the -b option,with value $OPTARG";;
	c) echo "Found the -c option";;
	d) echo "Found the -d option";;
	*) echo "Unknown option: $opt";;
	esac
done

# 没处理一个选项OPTIND就会增加1
shift $[ $OPTIND - 1 ]
count=1
for param in "$@"
do
	echo "Parameter $count: $param"
	count=$[ $count + 1 ]
done
