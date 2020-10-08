#!/bin/bash
# timed-input.sh

# TMOUT = 3  Also works, as of newer versions of Bash
TIMER_INTERRUPT=14
TIMELIMIT=3 # three seconds in this instance may be set to different value

function print_answer()
{
    if [ "$answer" = TIMEOUT ]
    then
        echo "$answer"
    else
        echo "Your favorite veggie is $answer"
        kill $! # $! is PID of last job running in backgroud
                # kills on-longer-needed TimeOn function
                # +running in backgroud
    fi
}

function time_on()
{
    sleep $TIMELIMIT && kill -s 14 $$ &
    # wait 3 seconds, then sends sigalarm to script
}

function int14_vector()
{
    answer="TIMEOUT"
    print_answer
    exit $TIMER_INTERRUPT
}

trap int14_vector $TIMER_INTERRUPT
# Timer interrupt (14) subverted for our purposes

echo "What is your favorite vegetable"
time_on
read answer
print_answer

