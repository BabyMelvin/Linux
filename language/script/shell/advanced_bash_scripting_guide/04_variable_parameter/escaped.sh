#!/bin/bash
# escaped.sh: escaped characters

#############################################################
### First, let's show some basic escaped-character usage. ###
#############################################################

# Escaping a newline.
# ------------------
echo ""

echo "This will print
as two lines"
# This will print
# as two line

echo "This will print \
as one lines"
# This will print
# as one line

echo; echo

echo "============="


echo "\v\v\v\v"      # Prints \v\v\v\v literally.
# Use the -e option with 'echo' to print escaped characters.
echo "============="
echo "VERTICAL TABS"
echo -e "\v\v\v\v"   # Prints 4 vertical tabs.
echo "=============="

echo "QUOTATION MARK"
echo -e "\042"       # Prints " (quote, octal ASCII character 42).
echo "=============="

