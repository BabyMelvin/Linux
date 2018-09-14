#!/bin/bash
# using pattern matching

# 可以用来样式匹配，表示以r开头的信息
if [[ $USER == r* ]]
then 
	echo "Hello $USER"
else
	echo "Sorry, I do not know you"
fi
