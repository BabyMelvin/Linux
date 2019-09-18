#!/bin/bash

# 运行方式 ./test < members.csv
outfile='members.sql'

IFS=','
while read lname fname address city state zip
do
    # 输出重定向，和输入重定向
    # cat命令取自EOF文件
    cat >> $outfile << EOF
    INSERT INTO members (lname,fname,address,city,state,zip) VALUES
('$lname','$fname','$address','$city','$state','$zip');
EOF
done < ${1} # 读取数据
