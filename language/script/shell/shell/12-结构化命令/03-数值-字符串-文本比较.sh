#!/bin/bash

# 1.数值比较
value1=10
value2=11

if [ $value1 -gt 5 ]
then
    echo "The value $value1 is greater than 5"
fi

if [ $value1 -eq $value2 ]
then
    echo "the values is equal"
else
    echo "the values is different"
fi

# 2.字符串比较

testuser=melvin

if [ $testuser = $USER ]
then
    echo "welcome $testuser"
fi

if [ $testuser != $USER ]
then
    echo "sorry $testuser, you have no permission"
fi

if [ -n $testuser ]
then
    echo "the string is not empty"
else
    echo "the string is empty"
fi

if [ -z $testuser ]
then 
    echo "the string is empty"
else
    echo "the string is not empty"
fi

# 3.文件比较
jump_dir=/home/melvin
if [ -d $jump_dir ]
then
    echo "the $jump_dir directory exists"
    cd $jump_dir
    ls
else
    echo "the $jump_dir directory is not exists"
fi

file_name=$HOME/hello

if [ -f $file_name ]
then
    if [ -s $file_name ]
    then
        echo "$file_name exists and have data in it"
    else
        echo "$file_name "
    fi
fi

# 4. 多条件测试

if [ -d $HOME ] && [ -w $ $HOME/hello ]
then
    echo "the file exists and you can write it"
fi
