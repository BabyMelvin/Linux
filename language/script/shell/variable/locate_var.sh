#!/bin/bash

# Usage: ./scriptename 1 2 3 4 5 6 7 8 9 10
MINPARAMS=10

echo

echo "The name of this script is \"$0\""

echo 

if [ -n "$1" ]     # 测试变量被引用
then 
   echo "Parameter #1 is $1" 
fi

# ...
if [ -n "${10}"]
then
    echo "Parameter #10 is ${10}"
fi

echo "---------------------------"
echo "All the command-line parameters are:"$*""
echo "All the command-line parameters are:"$@""

if [ $# -lt "$MINPARAMS" ]
then
    echo
    echo "This script needs at least $MINPARMS command-line arguments" 
fi
echo 
exit 0
