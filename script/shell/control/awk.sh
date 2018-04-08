#!/bin/bash

String=23skidool

echo ${String:2:4}
echo | awk '
{
    print substr("'"${String}"'",3,4)
}
'
exit 0
