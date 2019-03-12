#!/bin/bash

# redirecting the for output to a file
for file in /home/tiandi/*
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
