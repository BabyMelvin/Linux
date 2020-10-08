#!/bin/bash
# rmp-check.sh

# Queries an rpm file for description, listing and whether it can be installed
# save output to a file
#
# this script illustrates using a code block

SUCCESS=0
E_NOARGS=65

if [ -z "$1" ]
then
    echo "Usage:`basename $0` rpm-file"
    exit $E_NOARGS
fi

{ # begin code block
    echo
    echo "Archive Description:"
    rpm -qpi $1      # Query description
    echo
    echo "Archive Listing:"
    rpm -qpl $1      # Query listing
    echo
    rpm -i --test    # Query whether rpm file can be installed
    if [ "$?" -eq $SUCCESS ]
    then
        echo "$1 can be installed"
    else
        echo "$1 cannot be installed"
    fi
    echo             # End code block
} > "$1.test"        # Redirects output of everything in block to file

echo "Result of rpm test in file $1.test"

# see rpm page for explation of options
exit 0
