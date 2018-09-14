#!/bin/bash

var1=100
var2=45
# scale可用用来调节精度 bc用来精度计算
var3=`echo "scale=4; $var1 / $var2" | bc`
echo The answer for this is $var3

