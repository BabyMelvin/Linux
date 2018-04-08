#!/bin/bash

# 字符串长度 ${#string}
stringZ=abcdefeads

echo ${#stringZ}
echo `expr length "$stringZ" : '.*'`

# 正则表达式 ,substring 是一个正则表达式
expr match "$string" '$substring'
expr "$string" : '$substring'  

