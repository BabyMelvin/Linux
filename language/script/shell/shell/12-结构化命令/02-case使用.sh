#!/bin/bash

#using the case command

case $USER in
tiandi | barbar)
	echo "Welcome, $USER"
	echo "Pleas enjoy your visit";;
testing)
	echo "Special testing account";;
jessica)
	echo "Do not forget to logout when you are out";;
*)
	echo "Sorry, you are not allowed here";;
esac


while [ $# -gt 0 ] ; do
	case "$1" in
	--) shift ; break ;;
	-a) shift ; APPEND = yes ;;
	-n) shift ; BOARD_NAME = "${1%%_config}" ; shift ;;
	-t) shift ; TARGETS = "`echo $1 | sed 's:_: g'` ${TARGETS}" ; shift ;;
	*)  break ;;
	esac
done
