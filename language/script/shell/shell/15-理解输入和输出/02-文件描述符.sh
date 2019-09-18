#!/bin/bash

# 1.创建文件描述符

# 将test读写打开为3文件描述符
exec 3<> test
read line <&3
echo "Read: $line"
echo "This is the test line" >&3

# 2.关闭文件描述符
exec 3>&-

# 3.从重定向文件中恢复

exec 3>&1
exec 1>test

echo "This should store in output file"
echo "along with this line"

exec 1>&3
echo "Now things should be back to normal"
