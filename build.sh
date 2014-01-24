#!/bin/sh

# Add any build steps you need here
dd if=/dev/zero of=mem bs=757360 count=1
if [ Darwin = `uname -s` ]; then
cc -O0 -g newlevel0.c -o level0 -D_DARWIN_C_SOURCE
else
#cc -static -static-libgcc -O3 newlevel0.c -o level0 -DGNU_SOURCE
cc -O3 newlevel0.c -o level0 -DGNU_SOURCE -lm
fi

./level0 /usr/share/dict/words build
