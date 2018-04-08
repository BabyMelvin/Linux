#!/bin/bash

INTERVAL=5 # 超时间隔
timeout_read() {
    timeout=$1
    varname=$2
    old_tty_setting=`stty -g`
    stty -icanon min 0 time ${timeout}0
    eval read $varname   #或者仅仅读取varname变量
    stty "$old_tty_settings"
}

echo;echo -n "What's your name? Quick!"
timeout_read $INTERVAL your_name

echo 

if [ ! -z "$your_name" ]
then
    echo "Your name is $your_name"
else
    echo "Time out"
fi

echo 

exit 0
