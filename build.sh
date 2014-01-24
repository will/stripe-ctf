#!/bin/sh

# Add any build steps you need here
dd if=/dev/zero of=mem bs=10485760 count=1
cc -O3 newlevel0.c -o level0

./level0 /usr/share/dict/words build
