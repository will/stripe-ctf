#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "dict.c"

bool test2(char * word, size_t len) {
  long hash = 0;

  int sum = 0;
  size_t i;
  for (i =0;  word[i] != '\0'; i++) {
    sum += word[i] - 'a';
  }
  hash = sum;
  printf("%lu\n", hash);

  hash = hash << 5;
  hash += len;
  printf("%lu\n", hash);

  hash = hash << 5;
  hash += word[0]-'a';
  printf("%lu\n", hash);

  char c1 = 0;
  if (len > 1) c1 = word[1];
  hash = hash << 5;
  hash += c1-'a';
  printf("%lu\n", hash);


  char c2 = 0;
  if (len > 2) c2 = word[2];
  hash = hash << 5;
  hash += c2-'a';
  printf("\n%s(%zu): %lu", word,len, hash);

switch(hash) {
case 32768:
  if (strcmp(word,"a")) return 1;
  return 0;
case 65536:
  if (strcmp(word,"aa")) return 1;
  return 0;
case 11632651:
  if (strcmp(word,"aal")) return 1;
  return 0;
case 28475403:
  if (strcmp(word,"aalii")) return 1;
  return 0;
case 12681228:
  if (strcmp(word,"aam")) return 1;
  return 0;
case 71565329:
  puts("hello");
  printf("\n%u\n", strcmp(word,"aardvark"));
  if (!strcmp(word,"aardvark")) return 1;
  puts("ugh");
  return 0;
case 75759633:
  if (strcmp(word,"aardwolf")) return 1;
  return 0;
case 1146912:
  if (strcmp(word,"aba")) return 1;
  return 0;
case 3276832:
  if (strcmp(word,"abac")) return 1;
  return 0;
case 3309600:
  if (strcmp(word,"abaca")) return 1;
  return 0;
default:
  return 0;
}



}


int main(int argc,char *argv[]) {
   FILE *fp;
   char word[80];
   char lword[80];
   int c;
   int i;

  i = 0;
  bool cant = false;
  while(1) {
    c = getchar();
    if (c == EOF) { break; }
    if (c == '\n' || c == ' ') {
      word[i] = '\0';
      lword[i] = '\0';
      if (word[0] == '\0') {
        printf("%c",c);
      } else if (cant) {
        printf("<%s>%c",word,c);
        cant = false;
      } else if (test(lword, i)) {
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
  }

   return(0);
}
