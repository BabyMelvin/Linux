#!/bin/bash

# Clean up ,Version 3.0
# 此处插入代码，来打印错误信息，并且在不是root身份的时候退出

# 使用变量
LOG_DIR=/var/log
ROOT_UID=0          #$UID为0时候，用户才具有root用户权限
LINES=50            # 默认的保存行数
E_XCD=66            #不能修改目录
E_NOROOT=67         #非root用户将以error退出

# 当然是用root身份来运行
if [ "$UID" -ne "$ROOT_UID"]
then
    echo "Must be toot to run this script"
    exit $E_NOROOT
fi

if [ -n "$1" ]      #是否有命令行参数
then
    lines=$1
else
    lines=$LINES
fi

# 另一种检测命令行参数方法
E_WRONGARGS=65
case "$1" in
""      )   lines=50;;
*[!0-9]*)   echo "Usage:`basename $0` file-to-cleanup";exit $E_WRONGARGS;;
*       )   linws=$1;
esac

cd $LOG_DIR
if [ `pwd` != "$LOG_DIR" ]
then
    echo "Can't change to $LOG_DIR"
    exit $E_XCD
fi
# 更有效做法:
cd /var/log || {
    echo "Can't change to $LOG_DIR"
    exit $E_XCD
}
tail -$lines messages > mesg.temp  # 保存log file消息最后部分
mv mesg.temp messages              # 变更新的Log目录

#等价: `: > wtmp`和 `> wtmp`
cat /dev/null > wtmp
echo "LOGS cleaned up"

exit 0





