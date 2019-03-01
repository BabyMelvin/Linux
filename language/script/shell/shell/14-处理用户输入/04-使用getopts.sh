#!/bin/bash
# simple demonstration of the getopts command

# getopts命令会产生两个变量:OPTIND和OPTARG
# OPTIND初始值为1，表示下一个待处理的参数的索引，只要存在，getopts命令返回true,所以一般命令使用while循环
# OPTARG:getopts获取其期望的参数后存入的位置。如果不在期望内，则$opt被设为?并将该意外值存入OPTARG
#    如果$opt需要拥有具体设置而实际没有，则$opt被设为:并将丢弃设置的opt存入OPTARG
while getopts :ab:c opt
do
	case "$opt" in
	a) echo "Found the -a option";;
	b) echo "Found the -b option, with value $OPTARG";;
	c) echo "Found the -c option";;
	*) echo "Unknown option:$opt";;
	esac
done
