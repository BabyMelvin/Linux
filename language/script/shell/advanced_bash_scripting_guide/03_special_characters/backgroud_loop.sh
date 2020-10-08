#!/bin/bash
# backgroud_loop.sh

for i in {1..10}     # first loop
do
    echo -n "$i "
    sleep 1          # just for fun
done & # Run this loop in backgroud will sometimes execute after second loop

echo  # This 'echo' sometimes will not display

for i in {11..20}    # second loop
do
    echo -n "$i "
done

echo # This 'echo' sometimes will not display

exit 0
