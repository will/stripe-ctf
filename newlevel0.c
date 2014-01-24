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
#define MAX_DATA (10*1048576) // x1MB
#define ADDR 0x7ffff731b000

bool useWalloc = true;

int  currentOffset = 0;
void *walloc(int numBytes) {
  if (useWalloc) {
    char *ptr = (void *) ADDR + currentOffset;
    currentOffset += numBytes + (16 - (numBytes%16));
    return ptr;
  } else {
    return malloc(numBytes);
  }
}

#ifndef CRITBIT_H_
#define CRITBIT_H_

#ifdef _cplusplus
extern "C" {
#endif

  //typedef struct {
  //  void *root;
  //} critbit0_tree;

  //int critbit0_contains(critbit0_tree *t, const char *u);
  //int critbit0_insert(critbit0_tree *t, const char *u);
  //int critbit0_delete(critbit0_tree *t, const char *u);
  //void critbit0_clear(critbit0_tree *t);
  //int critbit0_allprefixed(critbit0_tree *t, const char *prefix,
  //                         int (*handle) (const char *, void *), void *arg);

#ifdef _cplusplus
};
#endif

#endif  // CRITBIT_H_
#define _POSIX_C_SOURCE 200112
#define uint8 uint8_t
#define uint32 uint32_t
/*2:*/


typedef struct{
  void*child[2];
  uint32 byte;
  uint8 otherbits;
}critbit0_node;

typedef struct{
  void*root;
}critbit0_tree;

/*:2*//*3:*/

int
critbit0_contains(critbit0_tree*t,const char*u){
  const uint8*ubytes= (void*)u;
  const size_t ulen= strlen(u);
  uint8*p= t->root;

  /*4:*/

  if(!p)return 0;

  /*:4*/

  /*5:*/

  while(1&(intptr_t)p){
    critbit0_node*q= (void*)(p-1);
    /*6:*/

    uint8 c= 0;
    if(q->byte<ulen)c= ubytes[q->byte];
    const int direction= (1+(q->otherbits|c))>>8;

    /*:6*/

    p= q->child[direction];
  }

  /*:5*/

  /*7:*/

  puts(p);
  return 0==strcmp(u,(const char*)p);

  /*:7*/

}

/*:3*//*8:*/

int critbit0_insert(critbit0_tree*t,const char*u)
{
  const uint8*const ubytes= (void*)u;
  const size_t ulen= strlen(u);
  uint8*p= t->root;

  /*9:*/

  if(!p){
    char*x = walloc(ulen+1);
    //int a= posix_memalign((void**)&x,sizeof(void*),ulen+1);
    memcpy(x,u,ulen+1);
    t->root= x;
    return 2;
  }

  /*:9*/

  /*5:*/

  while(1&(intptr_t)p){
    critbit0_node*q= (void*)(p-1);
    /*6:*/

    uint8 c= 0;
    if(q->byte<ulen)c= ubytes[q->byte];
    const int direction= (1+(q->otherbits|c))>>8;

    /*:6*/

    p= q->child[direction];
  }

  /*:5*/

  /*10:*/

  /*11:*/

  uint32 newbyte;
  uint32 newotherbits;

  for(newbyte= 0;newbyte<ulen;++newbyte){
    if(p[newbyte]!=ubytes[newbyte]){
      newotherbits= p[newbyte]^ubytes[newbyte];
      goto different_byte_found;
    }
  }

  if(p[newbyte]!=0){
    newotherbits= p[newbyte];
    goto different_byte_found;
  }
  return 1;

different_byte_found:

  /*:11*/

  /*12:*/

  newotherbits|= newotherbits>>1;
  newotherbits|= newotherbits>>2;
  newotherbits|= newotherbits>>4;
  newotherbits= (newotherbits&~(newotherbits>>1))^255;
  uint8 c= p[newbyte];
  int newdirection= (1+(newotherbits|c))>>8;

  /*:12*/


  /*:10*/

  /*13:*/

  /*14:*/

  //critbit0_node*newnode;
  //if(posix_memalign((void**)&newnode,sizeof(void*),sizeof(critbit0_node)))return 0;
  critbit0_node*newnode = walloc(sizeof(critbit0_node));

  char*x = walloc(ulen+1);
  //if(posix_memalign((void**)&x,sizeof(void*),ulen+1)){
  //free(newnode);
  //return 0;
  //}
  memcpy(x,ubytes,ulen+1);

  newnode->byte= newbyte;
  newnode->otherbits= newotherbits;
  newnode->child[1-newdirection]= x;

  /*:14*/

  /*15:*/

  void**wherep= &t->root;
  for(;;){
    uint8*p= *wherep;
    if(!(1&(intptr_t)p))break;
    critbit0_node*q= (void*)(p-1);
    if(q->byte> newbyte)break;
    if(q->byte==newbyte&&q->otherbits> newotherbits)break;
    uint8 c= 0;
    if(q->byte<ulen)c= ubytes[q->byte];
    const int direction= (1+(q->otherbits|c))>>8;
    wherep= q->child+direction;
  }

  newnode->child[newdirection]= *wherep;
  *wherep= (void*)(1+(char*)newnode);

  /*:15*/


  /*:13*/


  return 2;
}

/*:8*//*16:*/

int critbit0_delete(critbit0_tree*t,const char*u){
  const uint8*ubytes= (void*)u;
  const size_t ulen= strlen(u);
  uint8*p= t->root;
  void**wherep= &t->root;
  void**whereq= 0;
  critbit0_node*q= 0;
  int direction= 0;

  /*17:*/

  if(!p)return 0;

  /*:17*/

  /*18:*/

  while(1&(intptr_t)p){
    whereq= wherep;
    q= (void*)(p-1);
    uint8 c= 0;
    if(q->byte<ulen)c= ubytes[q->byte];
    direction= (1+(q->otherbits|c))>>8;
    wherep= q->child+direction;
    p= *wherep;
  }

  /*:18*/

  /*19:*/

  if(0!=strcmp(u,(const char*)p))return 0;
  free(p);

  /*:19*/

  /*20:*/

  if(!whereq){
    t->root= 0;
    return 1;
  }

  *whereq= q->child[1-direction];
  free(q);

  /*:20*/


  return 1;
}

/*:16*//*21:*/

static void
traverse(void*top){
  /*22:*/

  uint8*p= top;

  if(1&(intptr_t)p){
    critbit0_node*q= (void*)(p-1);
    traverse(q->child[0]);
    traverse(q->child[1]);
    free(q);
  }else{
    free(p);
  }

  /*:22*/

}

void critbit0_clear(critbit0_tree*t)
{
  if(t->root)traverse(t->root);
  t->root= NULL;
}

/*:21*//*23:*/

static int
allprefixed_traverse(uint8*top,
    int(*handle)(const char*,void*),void*arg){
  /*26:*/

  if(1&(intptr_t)top){
    critbit0_node*q= (void*)(top-1);
    int direction;
    for(direction= 0;direction<2;++direction)
      switch(allprefixed_traverse(q->child[direction],handle,arg)){
        case 1:break;
        case 0:return 0;
        default:return-1;
      }
    return 1;
  }

  /*27:*/

  return handle((const char*)top,arg);/*:27*/

}

int
critbit0_allprefixed(critbit0_tree*t,const char*prefix,
    int(*handle)(const char*,void*),void*arg){
  const uint8*ubytes= (void*)prefix;
  const size_t ulen= strlen(prefix);
  uint8*p= t->root;
  uint8*top= p;

  if(!p)return 1;
  /*24:*/

  while(1&(intptr_t)p){
    critbit0_node*q= (void*)(p-1);
    uint8 c= 0;
    if(q->byte<ulen)c= ubytes[q->byte];
    const int direction= (1+(q->otherbits|c))>>8;
    p= q->child[direction];
    if(q->byte<ulen)top= p;
  }

  /*:24*/

  /*25:*/

  size_t i;
  for(i= 0;i<ulen;++i){
    if(p[i]!=ubytes[i])return 1;
  }

  /*:25*/


  return allprefixed_traverse(top,handle,arg);
}

/*:23*/




static void*
critbit_copy(void* top){
  uint8*p = top;
  if(1&(intptr_t)p){
    critbit0_node*original= (void*)(p-1);
    critbit0_node*newnode = walloc(sizeof(critbit0_node));

    newnode->byte = original->byte;
    newnode->otherbits = original->otherbits;
    newnode->child[0] = critbit_copy(original->child[0]);
    newnode->child[1] = critbit_copy(original->child[1]);
    return newnode;
  }else{
    const char*u=p;
    const size_t ulen= strlen(u);
    char*x = walloc(ulen+1);
    memcpy(x,u,ulen+1);
    return x;
  }
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

  critbit0_tree tree = {0};
  // printf("\npointer %zu", tree.root);

  void **treeRoot = walloc(sizeof(void *));
  //printf("\nw pointer %zu", treeRoot);

  if (argc==3) {
#ifdef COMPACT
    useWalloc = false;
#endif
    // printf("\npointer %zu", tree.root);
    FILE *dict;
    dict = fopen(argv[1],"r");
    while (fscanf(dict, "%s", word) == 1) {
      if (!isupper(word[0])) { critbit0_insert(&tree, word); }
    }
    // printf("\npointer %zu", tree.root);


#ifdef COMPACT
    useWalloc = true;

    critbit0_tree newTree = {0};
    newTree.root = critbit_copy(tree.root);


    *treeRoot = newTree.root;
#else
    *treeRoot = tree.root;
#endif
      printf("offset: %d , newRoot: %zu", currentOffset, *treeRoot);

    return 0;
  } else {
    tree.root = *treeRoot;
    // printf("\npointer %zu", tree.root);
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
      i = 0;
      if (word[0] == '\0') {
        printf("%c",c);
      } else if (cant) {
        printf("<%s>%c",word,c);
        cant = false;
      } else if (critbit0_contains(&tree, lword)) {
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
    j++;
  }

  return(0);
}
