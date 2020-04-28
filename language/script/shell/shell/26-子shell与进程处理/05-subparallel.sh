#!/bin/bash

# 可以让shell进行并行处理

# 并行计算的用法
# 每个子shell都要搜索某个目录下root关键字匹配的行，排序后输出到某文件

(grep -r "root" /etc/* | sort > part1) &
(grep -r "root" /usr/local/* | sort > part2) &
(grep -r "root" /lib/* | sort > part3) &

wait  # 等待后台执行作业全部完成，再执行下面命令

# 将part1,part2,part3三个临时文件合并，排序后重定向到parttotal文件
cat part1 part2 part3 | sort > parttotal

echo "Run time of this script is :$SECONDS" # 输出脚本的执行时间
