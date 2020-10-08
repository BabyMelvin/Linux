#!/bin/bash
# clean up, version 3

LOG_DIR=/var/log
ROOT_UID=0        # only users with $UID 0 have root privilege
LINES=50          # default number of lines saved
E_XCD=86          # Can't change directory?
E_NOTROOT=87      # Non-root exit error

# Run as root, of course
if [ "$UID" -ne "$ROOT_UID" ]
then
    echo "Must be root to run this script"
    exit "$E_NOTROOT"
fi

if [ -n "$1" ]
# test whether command-line argument is present(non-empty)
then 
    line=$1
else
    line=$LINES  # default, if not specified on command-line
fi

# Stephane Chazelas suggests the following, as a better way of checking command-line arguments,
#  +but this is still a bit advanced for this stage of the tutorial.
#E_WRONGARGS=85   # Non-numerical argument(bad argument format)
#case "$1" in
#    ""      ) lines=50;;
#    *[!0-9]*) echo "Usage:`base name $0` lines-to-cleanup";
#                exit $E_WRONGARGS;;
#    *       ) lines=$1;;
#esac

cd $LOG_DIR

if [ `pwd` != "$LOG_DIR" ] # or if [ "$PWD" != "$LOG_DIR"]
                           # Not in /var/log?
then
    echo "Can't change to $LOG_DIR"
    exit $E_XCD
fi # double check if in right directory before messing with log file

# Far more efficient is:
# cd /var/log || {
#   echo "Can't change to necessary directory." >&2
#   exit $E_XCD;
# }

tail -n $lines message > mesg.temp   # save last section of message log file
mv mesg.temp messages                # Rename it as system log file

# cat /dev/null > messages
# No longer needed, as the above method is safer

cat /dev/null > wtmp # `: > wtmp` and `> wtmp` have the same effect
echo "Log files cleaned up."

# Note the there are other log files in /var/log not affected by this script
exit 0
