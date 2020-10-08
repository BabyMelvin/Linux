#!/bin/bash

function func()
{
    echo "This is a function"
}

declare -f # lists the function above

echo

declare -i var1 # var1 is an integer
var1=2367
echo "var1 declared as $var1"
var1=var1+1     # Integer delaration eliminates the need for 'let'
echo "var1 incremented by 1 is $var1"

# attempt to change variable declared as integer
var1=2367.1  # results in error message, with no change to variable
echo "var1 is still $var1"

echo

declare -r var2=13.36
echo "var2 declared as $var2" # attempt to change readonly variable
var2=13.37                    # generates error message, and exit from script

echo "var2 is still $var2"    # this line will not execute

exit 0                        # script will not exit here
