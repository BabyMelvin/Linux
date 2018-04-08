#!/bin/bash

# shift重新分配位置参数,其实吧所有位置参数向左移动一个位置.$1<--$2

until [ -z "$1" ]
do
    echo -n "$1 "
    shift
done

echo

exit
