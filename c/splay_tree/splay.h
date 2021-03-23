#ifndef __SPLAY_H_
#define __SPLAY_H_

#include "../common.h"

/* Splay-Tree Structure. */
typedef struct splay splay_t;
/* Function-pointer for deallocation of keys and items. */
typedef void (*freefunc_t)(void*);


/* Create new splay-tree. */
splay_t *splay_create(cmpfunc_t cmpfunc);

/* Destroy memory allocation for splay-tree. 
 * Destroy keys and/or items with provided function-pointers. 
 * Keys and/or items not destroyed if NULL is passed as function-pointer arguments. */
void splay_destroy(splay_t *splay, freefunc_t freekey, freefunc_t freeitem);

/* Return deepest depth of tree. */
int splay_depth(splay_t *splay);

/* Return number of items in tree. */
int splay_size(splay_t *splay);

/* Insert item and key into tree using compare function. 
 * If key already in splay-tree item will be overwritten. 
 * Return 1 if item and key inserted. Return 0 if item overwritten previous value. */
int splay_insert(splay_t *splay, void *key, void *item);

/* Search and return item in splay-tree with key. 
 * NULL returned if key not found in tree. */
void *splay_search(splay_t *splay, void *key);

/* Print splay-structure to .pdf using function-pointer to get string-representation of items in tree. */
void splay_print(splay_t *splay, strfunc_t strfunc);

#endif