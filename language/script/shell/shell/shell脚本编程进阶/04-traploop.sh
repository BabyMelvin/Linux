#!/bin/bash

# trap命令用于捕获INT信号的用法

# 一旦收到INT信号，执行双引号内的echo 命令
trap "echo 'You hit CONTROL+C '" INT

while :; do
    let count=count+1
    echo "This is the $count sleep"
    sleep 5   # 每5秒循环
done
