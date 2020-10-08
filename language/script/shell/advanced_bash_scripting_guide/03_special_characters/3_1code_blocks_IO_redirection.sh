#!/bin/bash
# Reading lines in /etc/fstab

File=/etc/fstab

{
    read line1
    read line2
} < $File

echo "First line in $File is:"
echo "$line1"

echo "Second line in $File is:"
echo "$line2"

exit 0

# parse the separate fileds of each line ?
# Hint: use awk or "set" Bash builtin
