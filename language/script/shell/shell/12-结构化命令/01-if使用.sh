# !/bin/bash

# 1.if后面能够执行date命令，相当于检测date返回值
if date
then
    echo "it is worked"
fi

# 换行
echo -e "\n"

# 2.if-else
testuser=ch
if grep $testuser /etc/passwd
then
    echo the files for user $testuser are:
    #ls -a /home/$testuser
    echo "it not worked"
else
    echo "the user name $testuser does not exist on this sytem"
fi

# 3.if-elif-else
# 注意空格要严格,方括号两边都要有
if [ $USER = "tiandi" ]
then
    echo "welcome $USER"
elif [ $USER = "ch" ]
then
    echo "welcome02 $USER"
else
    echo "welcome03 $USER"
fi
