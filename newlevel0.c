#define COMPACT
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>


// 10 MB of data that can be allocated
//define MAX_DATA 10485760 //10MB
//#define MAX_DATA (10*1048576) // x1MB
#define MAX_DATA 757360
#define ADDR 0x7ffff731b000

bool useWalloc = true;

int  currentOffset = 0;
void *walloc(int numBytes) {
  if (useWalloc) {
    char *ptr = (void *) ADDR + currentOffset;
    currentOffset += numBytes + (16 - (numBytes%16));
    //printf("size: %i off: %5x now: %lx max: %lx, diff: %lx\n", numBytes, currentOffset, (long)ADDR+currentOffset, (long)ADDR+MAX_DATA, ((long)MAX_DATA - (long)currentOffset));
    return ptr;
  } else {
    return malloc(numBytes);
  }
}
void *wcalloc(int numElem, int numBytes) {
  int total = numElem * numBytes;
  void *ptr = walloc(total);
  memset(ptr, 0, total);
 // printf("%i %i %p\n", numElem ,numBytes, ptr);
  return ptr;
}







//-----------------------------------------------------------------------------
// MurmurHash2, by Austin Appleby

// Note - This code makes a few assumptions about how your machine behaves -

// 1. We can read a 4-byte value from any address without crashing
// 2. sizeof(int) == 4

// And it has a few limitations -

// 1. It will not work incrementally.
// 2. It will not produce the same results on little-endian and big-endian
//    machines.

unsigned int murmurhash2(const void * key, int len, const unsigned int seed)
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value

	unsigned int h = seed ^ len;

	// Mix 4 bytes at a time into the hash

	const unsigned char * data = (const unsigned char *)key;

	while(len >= 4)
	{
		unsigned int k = *(unsigned int *)data;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}

	// Handle the last few bytes of the input array

	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
	        h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}
/*
 *  Copyright (c) 2012, Jyri J. Virkki
 *  All rights reserved.
 *
 *  This file is under BSD license. See LICENSE file.
 */

/*
 * Refer to bloom.h for documentation on the public interfaces.
 */

#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct bloom
{
  // These fields are part of the public interface of this structure.
  // Client code may read these values if desired. Client code MUST NOT
  // modify any of these.
  int entries;
  double error;
  int bits;
  int bytes;
  int hashes;

  // Fields below are private to the implementation. These may go away or
  // change incompatibly at any moment. Client code MUST NOT access or rely
  // on these.
  double bpe;
  unsigned char * bf;
  int ready;
};



static int bloom_check_add(struct bloom * bloom,
                           const void * buffer, int len, int add)
{
  if (bloom->ready == 0) {
    (void)printf("bloom at %p not initialized!\n", (void *)bloom);
    return -1;
  }

  int hits = 0;
  register unsigned int a = murmurhash2(buffer, len, 0x9747b28c);
  register unsigned int b = murmurhash2(buffer, len, a);
  register unsigned int x;
  register unsigned int i;
  register unsigned int byte;
  register unsigned int mask;
  register unsigned char c;

  for (i = 0; i < bloom->hashes; i++) {
    x = (a + i*b) % bloom->bits;
    byte = x >> 3;
    c = bloom->bf[byte];        // expensive memory access
    mask = 1 << (x % 8);

    if (c & mask) {
      hits++;
    } else {
      if (add) {
        bloom->bf[byte] = c | mask;
      }
    }
  }

  if (hits == bloom->hashes) {
    return 1;                   // 1 == element already in (or collision)
  }

  return 0;
}


int bloom_init(struct bloom * bloom, int entries, double error)
{
  bloom->ready = 0;

  if (entries < 1 || error == 0) {
    return 1;
  }

  bloom->entries = entries;
  bloom->error = error;

  double num = log(bloom->error);
  double denom = 0.480453013918201; // ln(2)^2
  bloom->bpe = -(num / denom);

  double dentries = (double)entries;
  bloom->bits = (int)(dentries * bloom->bpe);

  if (bloom->bits % 8) {
    bloom->bytes = (bloom->bits / 8) + 1;
  } else {
    bloom->bytes = bloom->bits / 8;
  }

  bloom->hashes = (int)ceil(0.693147180559945 * bloom->bpe);  // ln(2)

  bloom->bf = (unsigned char *)wcalloc(bloom->bytes, sizeof(unsigned char));
  if (bloom->bf == NULL) {
    return 1;
  }

  bloom->ready = 1;
  return 0;
}


int bloom_check(struct bloom * bloom, const void * buffer, int len)
{
  return bloom_check_add(bloom, buffer, len, 0);
}


int bloom_add(struct bloom * bloom, const void * buffer, int len)
{
  return bloom_check_add(bloom, buffer, len, 1);
}


void bloom_print(struct bloom * bloom)
{
  (void)printf("bloom at %p\n", (void *)bloom);
  (void)printf(" ->entries = %d\n", bloom->entries);
  (void)printf(" ->error = %f\n", bloom->error);
  (void)printf(" ->bits = %d\n", bloom->bits);
  (void)printf(" ->bits per elem = %f\n", bloom->bpe);
  (void)printf(" ->bytes = %d\n", bloom->bytes);
  (void)printf(" ->hash functions = %d\n", bloom->hashes);
}


void bloom_free(struct bloom * bloom)
{
  if (bloom->ready) {
    free(bloom->bf);
  }
  bloom->ready = 0;
}














#ifndef MAP_ANON
#define MAP_ANON 0x20
#endif


int main(int argc,char *argv[]) {
  int fd;
  fd = open("mem",  O_RDWR);
  if (argc==3) {
    void * s = mmap((void *) ADDR, MAX_DATA, PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_FIXED, fd, 0);
  } else {
    void * s = mmap((void *) ADDR, MAX_DATA, PROT_READ | PROT_WRITE,  MAP_PRIVATE | MAP_ANON | MAP_FIXED, -1, 0);
    //printf("mmap ret: %zi, %s\n", s, strerror(errno));
    int len = read(fd, (void *) ADDR, MAX_DATA);
    //printf("read ret: %d, %s\n", len, strerror(errno));
  }

  FILE *fp;
  char word[80];
  char lword[80];
  int c;
  int i;

  // printf("\npointer %zu", tree.root);

  struct bloom *bloom = walloc(sizeof(struct bloom));


  if (argc==3) {
    // printf("\npointer %zu", tree.root);
  bloom_init(bloom, 210687, 0.000001);
    FILE *dict;
    dict = fopen(argv[1],"r");
    while (fscanf(dict, "%s", word) == 1) {
      if (!isupper(word[0])) { bloom_add(bloom, word, strlen(word)); }
    }
    // printf("\npointer %zu", tree.root);


    //*treeRoot = tree.root;
    printf("memory used: %d\n", currentOffset);
    bloom_print(bloom);

    return 0;
  } else {
    //tree.root = *treeRoot;
     //printf("\npointer %zu", tree.root);
  }



  i = 0;
  int j = 0;
  bool cant = false;
  char input[163840];
  size_t lenread = 0;
  size_t readthistime;
  while ((readthistime = read(0, input + lenread, 163840)))
    lenread += readthistime;

  while(1) {
    if (j == lenread) { break; }
    c = input[j];
    if (c == '\n' || c == ' ') {
      word[i] = '\0';
      lword[i] = '\0';
      if (word[0] == '\0') {
        printf("%c",c);
      } else if (cant) {
        printf("<%s>%c",word,c);
        cant = false;
      } else if (bloom_check(bloom, lword, i)) {
        printf("%s%c",word,c);
      } else {
        printf("<%s>%c",word,c);
      }
      i = 0;
    } else {
      word[i] = c;
      lword[i] = tolower(c);
      if (c != '\n' && c != ' ' && (tolower(c) < 'a' || tolower(c) > 'z')) { cant = true; }
      i++;
    }
    j++;
  }

  return(0);
}
