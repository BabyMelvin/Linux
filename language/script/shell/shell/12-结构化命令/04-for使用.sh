# !/bin/bash

# 1.嵌套结构
for (( a=1; a<=3; a++))
do
    echo "start loop $a:"
    for (( b=1; b<=3; b++))
    do
        if [ $b -gt 3 ]
        then
            # break n,默认为 1,跳出的层数
            # continue 类似
            break 2
        fi
        echo "Inside loop:$b"
    done
done

#2.使用in

IFS.OLD=$IFS
# 取值
IFS=$'\n'
for entry in `cat /etc/passwd`
do
    echo "value in $entry -"
    IFS=:
    for value in $entry
    do
        echo "$value"
    done
done

