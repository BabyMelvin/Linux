#!/bin/bash

# 1.创建临时文件
tempfile=`mktemp test.XXXXXX`

exec 3>$tempfile

echo "This script writes to temp file $tempfile"
echo "This is the first line" >&3
echo "This is the second line" >&3
echo "This is the last line" >&3

exec 3>&-

echo "Done creating temp file.The contents are:"

cat $tempfile

rm -f $tempfile 2>/dev/null

# 2.创建临时目录

tempdir=`mktemp -d dir.XXXXXX`
cd $tempdir

tempfile1=`mktemp temp.XXXXXX`
tempfile2=`mktemp temp.XXXXXX`

exec 7> $tempfile1
exec 8> $tempfile2

echo "Sending data to directory $tempdir"
echo "This is a test line of data for $tempfile1" >&7
echo "This is a test line fo data for $tempfile2" >&8

# 3.在tmp目录下创建目录
tempfile=`mktemp -t tmp.XXXXXX`

echo "This is a test file" > $tempfile
echo "This is the second line of the test" >> $tempfile

echo "The temp is locate at : $tempfile"
cat $tempfile
rm -rf $tempfile

# 4.清零日志内容
cat /dev/null > logname

# 5.记录日记内容
# 能够输出到STDOUT，同事输出到文件中
tempfile=test
echo "This is the start of the test" | tee $tempfile
echo "This is the second line of the test" | tee -a $tempfile
echo "This is the end line of the test" | tee -a $tempfile
