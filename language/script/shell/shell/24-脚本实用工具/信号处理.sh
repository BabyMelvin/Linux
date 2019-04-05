#!/bin/bash


#命令kill $kill -l 列出所有的信号
#命令trap
#	1.自己处理(可以多个信号)
#      $trap "command;command;exit" 1 2
#	2.忽略信号
#      $trap "" 2
#	3.恢复信号
#      $trap 1 2

# 调用流程.
# What is your favorite vegetable
# TimerOn 等待三秒出发14信号，调用Int4Vector
          # 或者键值触发?
TIMELIMIT=3

PrintAnswer (){
    echo "PrintAnswer called"
    if [ "$answer" = TIMEOUT ]
    then
        echo "$answer"
    else
        echo "Your favorite veggie is $answer"
        kill $! # $! 变量是上一个后台运行作业的PID，不需要TimerOn函数,killd掉
    fi
}

TimerOn() {
    echo "TimerOn called"
    # 等待3秒给脚本发送一个信号
    sleep $TIMELIMIT && kill -s 14 $$ &
}

Int4Vector (){
    echo "Int4Vector called"
    answer="TIMEOUT"
    PrintAnswer
    exit 14
}

trap Int4Vector 14    # 自己处理信号

echo "What is your favorite vegetable"
TimerOn
read answer
PrintAnswer

exit 0


