#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

// Alphabet size (# of symbols)
#define ALPHABET_SIZE (52)
// Converts key current character into index
// use only 'a' through 'z' and lower case
#define CHAR_TO_INDEX(c) ((int)c - (int)'a')

// trie node
typedef struct trie_node trie_node_t;
struct trie_node
{
    int value;
    trie_node_t *children[ALPHABET_SIZE];
};

// trie ADT
typedef struct trie trie_t;
struct trie
{
    trie_node_t *root;
    int count;
};

// Returns new trie node (initialized to NULLs)
trie_node_t *getNode(void)
{
    trie_node_t *pNode = NULL;

    pNode = (trie_node_t *)malloc(sizeof(trie_node_t));

    if( pNode )
    {
        int i;

        pNode->value = 0;

        for(i = 0; i < ALPHABET_SIZE; i++)
        {
            pNode->children[i] = NULL;
        }
    }

    return pNode;
}

// Initializes trie (root is dummy node)
void initialize(trie_t *pTrie)
{
    pTrie->root = getNode();
    pTrie->count = 0;
}

// If not present, inserts key into trie
// If the key is prefix of trie node, just marks leaf node
void insert(trie_t *pTrie, char key[])
{
    int level;
    int length = strlen(key);
    int index;
    trie_node_t *pCrawl;

    pTrie->count++;
    pCrawl = pTrie->root;

    for( level = 0; level < length; level++ )
    {
        //index = CHAR_TO_INDEX(tolower(key[level]));
        index = CHAR_TO_INDEX(key[level]);
        if( !pCrawl->children[index] )
        {
            pCrawl->children[index] = getNode();
        }

        pCrawl = pCrawl->children[index];
    }

    // mark last node as leaf
    pCrawl->value = pTrie->count;
}

// Returns non zero, if key presents in trie
int search(trie_t *pTrie, char key[])
{
    int level;
    int length = strlen(key);
    int index;
    trie_node_t *pCrawl;

    pCrawl = pTrie->root;

    for( level = 0; level < length; level++ )
    {
        index = CHAR_TO_INDEX(tolower(key[level]));

        if( !pCrawl->children[index] )
        {
            return 0;
        }

        pCrawl = pCrawl->children[index];
    }

    return (0 != pCrawl && pCrawl->value);
}

int main(int argc,char *argv[]) {
   FILE *fp;
   char word[80];
   int c;
   int i;

//char keys[][8] = {"the", "a", "there", "answer", "any", "by", "bye", "Their"};
trie_t trie;
char output[][32] = {"Not present in trie", "Present in trie"};
initialize(&trie);
// Construct trie
//for(int i = 0; i < ARRAY_SIZE(keys); i++)
//{
//    insert(&trie, keys[i]);
//}

FILE *dict;
dict = fopen(argv[1],"r");
while (fscanf(dict, "%s", word) == 1) {
  if (!isupper(word[0])) { insert(&trie, word); }
}


// Search for different keys
//printf("%s --- %s\n", "the", output[search(&trie, "the")] );
////printf("%s --- %s\n", "these", output[search(&trie, "these")] );
//printf("%s --- %s\n", "their", output[search(&trie, "their")] );
//printf("%s --- %s\n", "thaw", output[search(&trie, "thaw")] );


  i = 0;
  bool cant = false;
  while(1) {
    c = getchar();
    if (c == EOF) { break; }
    if (c == '\n' || c == ' ') {
      word[i] = '\0';
      i = 0;
      if (word[0] == '\0') {
        printf("%c",c);
      } else if (cant) {
        printf("<%s>%c",word,c);
        cant = false;
      } else if (search(&trie, word)) {
        printf("%s%c",word,c);
      } else {
        printf("<%s>%c",word,c);
      }
    } else {
      word[i] = c;
      if (c != '\n' && c != ' ' && (tolower(c) < 'a' || tolower(c) > 'z')) { cant = true; }
      i++;
    }
  }
  //while (scanf("%s", word) != EOF) {
  //  printf("<%s>",word);
  //}
  // while( fgets(str, 60, stdin) ) {
  //    printf("%s",str);
  // }

   return(0);
}
