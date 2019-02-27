# !/bin/bash

# 1.与用expr运算
var1=10
var2=20
var3=`expr $var2 / $var1`
echo "var3 = $var3"

# 2.使用中括号

var4=$[$var1 * $var2]
echo "var4 = $var4"

# 3.使用内联定向计算

var3=33.2
var5=`bc << EOF
scale=4
a1 = $var3 * $var3
b1 = $var3 + $var1
a1 + b1
EOF`
echo "var5 = $var5"

# 4.处理浮点
val=`echo "scale=4;10 /3" | bc` 
