#!/bin/sh

# Add any build steps you need here
dd if=/dev/zero of=mem bs=504928 count=1
if [ Darwin = `uname -s` ]; then
cc -O0 -g newlevel0.c -o level0 -D_DARWIN_C_SOURCE
else
cc -O3 newlevel0.c -o level0 -DGNU_SOURCE -lm
#cc -O3 -static -static-libgcc -s newlevel0.c -o level0 -DGNU_SOURCE -lm
fi

./level0 /usr/share/dict/words build
