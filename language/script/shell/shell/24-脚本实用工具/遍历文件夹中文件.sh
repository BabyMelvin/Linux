#!/bin/bash

KEY_WORD=$1
FILE_PREFIX="LOG@"
OUT_FILENAME="silead_log.txt"

if [ -z $KEY_WORD ]
then
    KEY_WORD="silead"
fi

case $KEY_WORD in
    h | H | -h | -H)
        echo "Usage: hisi.sh keyword"
        echo "keyword Regular Expression such as: silead or sil|silead"
        exit 0
        ;;
esac

if [ -e $OUT_FILENAME ]
then
    rm $OUT_FILENAME
fi

function judge_dir() {
    for file in `ls $1`
    do
        if [ -d $1/$file ]
        then
            judge_dir $1/$file $2
        else
            local path=$1"/"$file
            #echo "path ++++++++++++++ $path"
            if [ -f $path ]  && [[ $file == $FILE_PREFIX* ]] 
            then 
                echo "prefix $ $FILE_PREFIX: $path"
                echo "$2"
                grep -E $2 $path >> $OUT_FILENAME
            fi
        fi
    done
}

judge_dir . $KEY_WORD 
