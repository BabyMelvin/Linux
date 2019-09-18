#!/bin/bash

# 1.简单使用 read
echo -n "Enter your name:"
read name
echo "Hello $name, welcome to my program"

read -p "Please enter your age: " age
days=$[ $age * 365 ]
echo "That makes you over $days days old"


# 2.指定多个变量
#指定多个变量，输入的每个数据值都会分配给表中的下一个变量，如果用完了，就全分配各最后一个变量
read -p "Please enter name:" first last
echo "Checking data for $last. $first..."

# 3.read默认的变量REPLY
#如果不指定变量，read命令就会把它收到的任何数据都放到特殊环境变量REPLY中
read -p "Enter a number:"
factorial=1
for (( count=1; count<=$REPLY; count++))
do
	factorial=$[ $factorial * $count ]
done
echo "The factorial of $REPLY is $factorial"

# 4.超时和计数
if read -t 5 -p "Please enter your name:" name
then
	echo "Hello, $name, welcome to my script"
else
	# 起到换行的作用
	echo
    # -n 和值1 使用，表示命令接收单个字符后退出，无需要按回车键
	read -n1 -p "Do you want to continue [Y/N]?" answer
	case $answer in
	Y | y) echo
		   echo "Fine, continue on...";;
	N | n) echo
		   echo "OK，goodbye";;
	*) echo
	   echo "OK, wrong, goodbye"
	esac
	echo "Sorry, this is the end of the script"
fi

# 5.隐藏的方式输入
# 输入密码的时候

read  -s -p "Enter your password:" pass
echo 
echo "Is your password really $pass?"

# 6.读取文件输入

count=1
cat test | while read line
do
    echo "Line $count: $line"
    count=$[ $count + 1 ]
done
echo "Finished processing the file"

