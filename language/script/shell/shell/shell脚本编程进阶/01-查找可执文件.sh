#!/bin/bash

# 查找所有可执行文件 (PATH中的)
 
IFS=:
for folder in $PATH
do
    echo "$folder"
    for file in $folder/*
    do
        if [ -x $file ]
        then 
            echo " $file"
        fi
    done
done

