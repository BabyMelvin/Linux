#/bin/bash

# TMOUT环境变量，设置为非零值时候，经过time秒后，shell提示超时.导致logout

TMOUT=3 # 提示输入事件3秒
echo "what is your favorite song?"
echo "Quickly now,you only have $TMOUT seconds to answer!"

read song
if [ -z "$song" ]
then
    song="(no answer)"
fi 

echo "Your favorite song is $song"


