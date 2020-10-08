#!/bin/bash

# Erratic behavior of the "$*" and "$@" internal Bash variables,
#+ depending on whether or not they are quoted.
#  Demonstrates inconsistent handling of word splitting and linefeeds.

set -- "First one" "second" "third:one" "" "Fifth: :one"
# Setting the script arguments,$1,$2,$3,etc

echo
echo 'IFS unchanged, using "$*"'
c=0
for i in "$*"
do
    echo "$((c+=1)):[$i]"
done
echo ---

exit 0
