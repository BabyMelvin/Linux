#!/bin/bash

# getopt 命令格式:getopt optstring parameters
# optstring:需要参数值选项字母后面加一个冒号
# getopt b:cd -b test1 -cd test2 test3
# getopt 添加多余的选项会导致报错,加-q 忽略

# 1.用getopt命令生成的格式化版本来代替已有的命令行选项和参数

# set 命令能够做到，-- 将命令行参数替换成set命令的命令行值
set -- `getopt -q ab:c "$@"`
while [ -n "$1" ]
do
	case "$1" in
	-a) echo "Found the -a option";;
	-b) param="$2"
		echo "Found the -b option,with parameter value $param"
		shift;;
	-c) echo "Found the -c option";;
	--) shift
		break;;
	*) echo "$1 is not an option";;
	esac
	shift
done

count=1
for param in "$@"
do
	echo "Parameter #$count: $param"
	count=$[ $count+1 ]
done

# 能够正确处理：./test.sh -a -b test1 -cd test2 test3 test4
# 注意：不擅长处理带空格和引号的参数
