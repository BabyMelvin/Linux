#!/bin/bash

# 可以对循环输出使用管道或进行重定向，done命令后面添加处理命令来实现

# shell会将for命令结果输出到Output.txt文件而不是屏幕上
for file in /home/melvin/*
do
	if [ -d "$file" ]
	then
		echo "$file is a directory"
	else
		echo "$file is a file"
	fi
done > output.txt

# piping a loop to another command

for state in "North Dakota" Connecticut
do
	echo "$state is next place to go"
done | sort
echo "This completes our travels"


# 重定向到文件,既有到终端又到hello.txt文件中

echo "hello world\n" 2>&1 | tee hello.txt
