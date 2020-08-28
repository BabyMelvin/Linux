#!/bin/bash

gcc -c wrap.c test.c

gcc -Wl,--wrap,malloc -o test wrap.o test.o
