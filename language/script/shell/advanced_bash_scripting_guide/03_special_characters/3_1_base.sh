#!/bin/bash

if false 
then :  # do nothing and branch ahead
else    # or else ...
    echo "hello :"
fi

# provide a placeholder where binary operation is expected
: ${username=`whoami`}
# ${username=`whoami`} Give an error without the leading:unless "username" is a command or builtin...

:${1?"Usage: $0 ARGUMENT"} # From "usage-message.sh example script.

: > data.xxx # File "data.xxx" now empty
# Same effect as cat /dev/null > data.xxx
# However,this does not fork a new process, since ":" is a builtin

echo *  # match every filename in a given directory

((var0=var<98?9:21))   # ? c style trinary operator

(a=hello; echo $a)  # start a sub shell

echo {a..e} # extened brace expansion output:a b c d e
base64_charset=({A..Z} {a..z} {0..9} + / =)

# {} place holder for text
# used after xargs -i, {} are placeholder for output text
ls . | xargs -i -t cp ./{} $1

# ls . | ./uppercase.sh

# TODO stdout in a pipe must be read as stdin of the next. if not the case the data stream 
# will block, and the pipe will not behave as expected
cat file1 file2 | ls -l | sort
# The output from "cat file1 file2" disappers

variable="initial_value"
echo "new value" | read variable
echo "variable=$variable"    # variable = initial_value
                             # called a broken pipe, sends SIGPIPE signal
#############################################

########################### - #####################################
# cat - # echoes stdin
(cd /source/directory && tar cf - .) | (cd /dest/directory && tar xpvf -)
# Move entire file tree from one directory to another
# [courtesy Alan Cox <a.cox@swansea.ac.uk>, with a minor change]

# 1) cd /source/directory
#    Source directory, where the files to be moved are.
# 2) &&
#   "And-list": if the 'cd' operation successful,
#    then execute the next command.
# 3) tar cf - .
#    The 'c' option 'tar' archiving command creates a new archive,
#    the 'f' (file) option, followed by '-' designates the target file
#    as stdout, and do it in current directory tree ('.').
# 4) |
#    Piped to ...
# 5) ( ... )
#    a subshell
# 6) cd /dest/directory
#    Change to the destination directory.
# 7) &&
#   "And-list", as above
# 8) tar xpvf -
#    Unarchive ('x'), preserve ownership and file permissions ('p'),
#    and send verbose messages to stdout ('v'),
#    reading data from stdin ('f' followed by '-').
#
#    Note that 'x' is a command, and 'p', 'v', 'f' are options.
#
# Whew!

# More elegant than, but equivalent to:
#   cd source/directory
#   tar cf - . | (cd ../dest/directory; tar xpvf -)
#
#     Also having same effect:
# cp -a /source/directory/* /dest/directory
#     Or:
# cp -a /source/directory/* /source/directory/.[^.]* /dest/directory
#     If there are hidden files in /source/directory.

bunzip2 -c linux-2.6.16.tar.bz2 | tar xvf -
#  --uncompress tar file--      | --then pass it to "tar"--
# If "tar" has not been patched to handle "bunzip2",this needs to be done in two discrete steps, using a pipe.
# The purpose of the exercise is to unarchive "bzipped" kernel source.

# bash$ echo "whatever" | cat -
# whatever

# Where a filename is expected, - redirects output to stdout (sometimes seen with tar cf), or accepts input from stdin, rather than from a file. This is a method of using a file-oriented utility as a filter in a pipe.

# The "-" can be used to pipe stdout to other commands. 
grep Linux file1 | diff file2 -
