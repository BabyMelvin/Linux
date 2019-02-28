#!/bin/bash

# 1.使用IFS
file="state"

#更改字段分隔符，使其只能识别换行符
IFS=$'\n'

#处理长脚本是，在一个地方修改了该值，然后可能忘了修改过该值
#IFS.OLD=$IFS
#IFS=$'\n'
#具体代码
#IFS=$IFS.OLD

for state in `cat $file`
do
	echo "Visit beautiful $state"
done

# 2.changing the IFS value

IFS.OLD=$IFS
IFS=$'\n'
for entry in `cat /etc/passwd`
do	
	echo "Values in $entry -"
	IFS=:
	for value in $entry
	do
		echo " $value"
	done
done


