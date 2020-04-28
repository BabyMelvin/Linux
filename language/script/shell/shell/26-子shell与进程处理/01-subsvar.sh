#!/bin/bash

#############
#圆括号和BASH_SUBSHELL
#   1.子shell变量对父shell是不可见的
#   2.BASH_SUBSHELL一次记录层级，父：0，子：1，孙：2
#############

echo "The level of father shell is:$BASH_SUBSHELL" # 打印父shell的层次
outervar=OUTER

( # 进入子shell
echo "The level of SubShell is :$BASH_SUBSHELL"
innerval=INNER
echo "innerval=$innerval"
echo "outervar=$outervar"
)

# 回到父shell中
echo "The level of father shell is:$BASH_SUBSHELL" # 打印父shell的层次

if [ -z $innerval ]
then
    echo "The \$innerval is not defined in main body"
else
    echo "The \$innerval is defined in main body"
fi
