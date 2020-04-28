#!/bin/bash

#######
#  子shell环境变量不影响父shell
######

( # 进入子shell
    set -C    #开启-C选项，防止重定向覆盖文件
    : > outputnull # 试图冒号清空outputnull文件
)

# 在父shell中覆盖一个文件，测试-C是否生效
cat 03-subsenv.sh > outputnull

