#!/bin/bash

# 1.测试命令
var1=10
while [ $var1 -gt 0 ]
do 
    echo $var1
    var1=$[ $var1 -1 ]
done

#2.while内嵌for循环

while [ $var1 -ge 0 ]
do
    echo "outer loop: $var1"
    for (( var2=1; $var2<3; var2++))
    do
        var3=$[ $var2*$var1 ]
        echo "Inner loop: $var1 * $var2 = $var3"
    done
    var1=$[ $var1 -1 ]
done

# 3.测试多条命令

while echo $var1
    [ $var1 -ge 0 ]
do
    echo "this is inside the loop"
    var1=$[ $var1 -1 ]
done
