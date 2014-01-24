touch mem
touch level0
rm level0
rm mem && dd if=/dev/zero of=mem bs=10485760 count=1 && cc -O0 -g newlevel0.c -o level0 -D_DARWIN_C_SOURCE && ./level0 words build
