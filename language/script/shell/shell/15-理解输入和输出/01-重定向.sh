#!/bin/bash

# 1.临时重定向
echo "This is an error" >&2
echo "This is another error"
echo "This is also an error" >&2

# 2.永久重定向

# 重定向shell某个特定的文件描述符
exec 2>test

ls badtest
echo "This is the test of redirecting all output"
echo "from a script to another file"

exec 1>test1
echo "This is the end of the script"
echo "but this should go to the testerror file" >&2

# 3.重定向输入
# 从test中读取数据，而不是从STDIN中读取数据
exec 0< test
count=1
while read line
do
    echo "Line #$count : $line"
    count= $[ $count + 1]
done
