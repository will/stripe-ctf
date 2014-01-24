#!/bin/sh

# Add any build steps you need here
dd if=/dev/zero of=mem bs=10485760 count=1
cc -static-libgcc -O2 newlevel0.c -o level0

./level0 /usr/share/dict/words build
