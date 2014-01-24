// 10 MB of data that can be allocated
#define MAX_DATA 10485760
#define ADDR 0x7ffff731b000

int  currentOffset = 0;

void *walloc(int numBytes) {
    char *ptr = (void *) ADDR + currentOffset;
    currentOffset += numBytes + (16 - (numBytes%16));
    return ptr;
}


#include<stdlib.h>
#include<limits.h>
#include<stdarg.h>
typedef unsigned int (*hashfunc_t)(const char *);
typedef struct {
	size_t asize;
	unsigned char *a;
	size_t nfuncs;
	hashfunc_t *funcs;
} BLOOM;


#define SETBIT(a, n) (a[n/CHAR_BIT] |= (1<<(n%CHAR_BIT)))
#define GETBIT(a, n) (a[n/CHAR_BIT] & (1<<(n%CHAR_BIT)))

BLOOM *bloom_create(size_t size, size_t nfuncs, ...)
{
	BLOOM *bloom;
	va_list l;
	int n;

	if(!(bloom=walloc(sizeof(BLOOM)))) return NULL;
	if(!(bloom->a=calloc((size+CHAR_BIT-1)/CHAR_BIT, sizeof(char)))) {
		free(bloom);
		return NULL;
	}
	if(!(bloom->funcs=(hashfunc_t*)walloc(nfuncs*sizeof(hashfunc_t)))) {
		free(bloom->a);
		free(bloom);
		return NULL;
	}

	va_start(l, nfuncs);
	for(n=0; n<nfuncs; ++n) {
		bloom->funcs[n]=va_arg(l, hashfunc_t);
	}
	va_end(l);

	bloom->nfuncs=nfuncs;
	bloom->asize=size;

	return bloom;
}

int bloom_destroy(BLOOM *bloom)
{
	free(bloom->a);
	free(bloom->funcs);
	free(bloom);

	return 0;
}

int bloom_add(BLOOM *bloom, const char *s)
{
	size_t n;

	for(n=0; n<bloom->nfuncs; ++n) {
		SETBIT(bloom->a, bloom->funcs[n](s)%bloom->asize);
	}

	return 0;
}

int bloom_check(BLOOM *bloom, const char *s)
{
	size_t n;

	for(n=0; n<bloom->nfuncs; ++n) {
		if(!(GETBIT(bloom->a, bloom->funcs[n](s)%bloom->asize))) return 0;
	}

	return 1;
}

unsigned int sax_hash(const char *key)
{
	unsigned int h=0;

	while(*key) h^=(h<<5)+(h>>2)+(unsigned char)*key++;

	return h;
}

unsigned int sdbm_hash(const char *key)
{
	unsigned int h=0;
	while(*key) h=(unsigned char)*key++ + (h<<6) + (h<<16) - h;
	return h;
}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>


int main(int argc,char *argv[]) {
int fd;
fd = open("mem",  O_RDWR);
ssize_t s = mmap((void *) ADDR, MAX_DATA, PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_FIXED, fd, 0);
//printf("ret: %zi, %s\n", s, strerror(errno));
   FILE *fp;
   char word[80];
   char lword[80];
   int c;
   int i;

   BLOOM *bloom;
 // printf("\npointer %zu", tree.root);

   BLOOM **treeRoot = walloc(sizeof(BLOOM*));
//printf("\nw pointer %zu", treeRoot);
puts("hello");

if (argc==3) {
   bloom = bloom_create(25000000, 2, sax_hash, sdbm_hash);
    printf("\npointer %zu", bloom);
    FILE *dict;
    dict = fopen(argv[1],"r");
    while (fscanf(dict, "%s", word) == 1) {
      if (!isupper(word[0])) { bloom_add(bloom, word); }
    }
   printf("\npointer %zu", bloom);
   *treeRoot = bloom;
   return 0;
} else {
   puts("hi");
   printf("\npointer %zu\n", treeRoot);
   bloom = treeRoot+16;
   printf("\npointer %zu", bloom);
}


  i = 0;
  bool cant = false;
  while(1) {
    c = getchar();
    if (c == EOF) { break; }
    if (c == '\n' || c == ' ') {
      word[i] = '\0';
      lword[i] = '\0';
      i = 0;
      if (word[0] == '\0') {
        printf("%c",c);
      } else if (cant) {
        printf("<%s>%c",word,c);
        cant = false;
      } else if (bloom_check(bloom, lword)) {
        printf("%s%c",word,c);
      } else {
        printf("<%s>%c",word,c);
      }
    } else {
      word[i] = c;
      lword[i] = tolower(c);
      if (c != '\n' && c != ' ' && (tolower(c) < 'a' || tolower(c) > 'z')) { cant = true; }
      i++;
    }
  }

   return(0);
}
