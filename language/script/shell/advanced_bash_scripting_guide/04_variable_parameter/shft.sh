#!/bin/bash
# shft.sh:Using shfit to step through all the positional parameters

# name this script something like shft.sh
# + and invoke it with some parameters
# for example:
#       sh shft.sh a b c def 83 barndoor

until [ -z "$1" ]
do
    echo -n "$1"
    shift # add stride
done

echo  # Extra linefeed

# parameters move

exit
