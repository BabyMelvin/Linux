#!/bin/bash

#   #       注释用
#   ；      一行写多个命令
#   ;;      case终止选项
#   `       命令行
#   :       等价NOP,pass
:
echo $? # 0
while :     # 死循环
do
    operations ...
done

if conditon
then :
else
    some-action
fi

:   ${username=`whoami`}
    ${username=`whoami`}    #这样将会报错,除非是命令或者内建命令

#参数替换来评估字符串变量
: ${HOSTMAIL?} ${USER?} ${MAIL?} # 如果一个或多个环境变量没有被设置会打印错误信息




