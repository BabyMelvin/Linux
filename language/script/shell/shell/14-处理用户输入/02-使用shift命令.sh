#!/bin/bash
# shift n 移动变量

# 1.shift命令使用
# shift命令，$3->$2,$1会被删除,$2->$1
count=1
while [ -n "$1" ]
do
	echo "Parameter #$count = $1"
	count=$[ $count+1 ]
	shift
done

echo -e "\n"

# demonstrating a multi-position shift
echo "The original parameters : $*"
shift 2
echo "Here's the new first parameter: $1"

# 2.选项和参数进行分离
# shell使用双破折线来表明选项列表结束,其他表示参数
# ./test -a -c -a -- test1 test2 test3
echo

while [ -n "$1"]
do
    case "$1" in
        -a) echo "Found the -a option";;
        -b) echo "Found the -b option";;
        -c) echo "Found the -c option";;
        --) shift
            break ;;
        *) echo "$1 is not an option" ;;
    esac
    shift
done

count=1
for param in "$@"
do
    echo "Parameter #$count:$param"
    count=$[ $count + 1]
done

# 3.处理带值的选项
while [ -n "$1" ]
do 
	case "$1" in
	-a) echo "Found the -a option";;
	-b) param="$2"
		echo "Found the -b option, with parameter value $param"
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
	echo "Parameter #$count : $param"
	count=$[ $count + 1 ]
done
