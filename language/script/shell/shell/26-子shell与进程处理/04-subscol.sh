#!/bin/bash

# 测试shell变量是否定义过
# 没定义：unbound variable
# bash执行export var=1000

# :冒号很重要，只做变量展开不做计算
if (set -u; : $var) # ：后面有空格
then
    echo "Variable is set"
fi
