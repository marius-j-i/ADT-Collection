/* Author: Marius Ingebrigtsen */

#ifndef __AVL_H_
#define __AVL_H_

#include "../common.h"

/* Structure for AVL-tree. */
typedef struct avl avl_t;
/* Function-pointer for freeing arbitrary items and keys in structure. */
typedef void (*freefunc_t)(void*);
/* Function-pointer for string-representation of items. */
typedef char* (*strfunc_t)(void*);

/* Create new AVL. */
avl_t *avl_create(cmpfunc_t cmpfunc);

/* Destroy AVL-structure, with optional deallocation function-pointers for keys and items. */
void avl_destroy(avl_t* avl, freefunc_t freekey, freefunc_t freeitem);

/* Return number of entries into AVL. */
int avl_size(avl_t *avl);

/* Insert item into structure using key to compare. 
 * If key is already in AVL, argument-item will overwrite existing item. 
 * Return 1 if insertion successfull, return 0 if key already in AVL. */
int avl_insert(avl_t *avl, void *key, void *item);

/* Search AVL using key and return item with key. 
 * Return NULL if item not in AVL. */
void *avl_search(avl_t *avl, void *key);

/* Sort AVL iteration using cmpfunc to compare. 
 * If sorted after an iterator is created, iterator position in sequence may be changed. 
 * To fix this call avl_resertiterator(). */
void avl_sort(avl_t *avl);

/* Print AVL-structure using plot-library with string-representations of items using provided function-pointer. */
void avl_print(avl_t *avl, char *pdfname, strfunc_t strfunc);


/* Iteration: */
typedef struct avl_iterator avl_iterator_t;

/* Create new iterator. */
avl_iterator_t *avl_createiterator(avl_t *avl);

/* Deallocate iterator. */
void avl_destroyiterator(avl_iterator_t *iterator);

/* Return 1 if iterator not at end of sequence, return 0 otherwise. */
int avl_hasnext(avl_iterator_t *iterator);

/* Iterate and return next item in sequence. */
void *avl_next(avl_iterator_t *iterator);

/* Set iterator at start of iteration. */
void avl_resetiterator(avl_iterator_t *iterator);

#endif