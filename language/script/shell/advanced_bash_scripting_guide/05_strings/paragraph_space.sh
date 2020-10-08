#!/bin/bash
# paragraph_space.sh

#  Insert a blank line between paragraphs of a single-spaced text file.
#  Usage:$0 <FILENAME

MINLEN=60   # change this value?it's a judgement call

while read line
do
    echo "$line"
    len=${#line}
    if [[ "$len" -lt "$MINLEN" && "$line" =~[*{\.}]]$ ]
    then echo
    fi
done

exit
