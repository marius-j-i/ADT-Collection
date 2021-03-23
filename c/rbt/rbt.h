#ifndef __RBT_H_
#define __RBT_H_

#include "../common.h"

typedef struct rbt rbt_t;
/* Function-pointer for deallocation of keys and items. */
typedef void (*freefunc_t)(void*);
/* Funciton-pointer for string-representation of items. */
typedef char* (*strfunc_t)(void*);


/* Return new rbt. */
rbt_t *rbt_create(cmpfunc_t cmpfunc);

/* Destroy rbt. 
 * If function pointer is not NULL, then keys and items is destroyed using given function. */
void rbt_destroy(rbt_t *rbt, freefunc_t freekey, freefunc_t freeitem);

/* Return number of items in rbt if 'size'-argument is 1. 
 * Return depth of tree otherwise. */
int rbt_size(rbt_t *rbt, int size);

/* Insert item using cmpfunc to compare keys. 
 * If key is already in rbt, then item will overwrite existing value.
 * Return 1 if inserted, 0 if overwritten. */
int rbt_insert(rbt_t *rbt, void *key, void *item);

/* Search rbt using key and return item with key. 
 * Return NULL if item not in rbt. */
void *rbt_search(rbt_t *rbt, void *key);

/* Remove item associated with key. 
 * Return 1 if removed, 0 if key not associated with item. 
 * Optional function-pointers for deallocation of key and item, pass NULL to avoid deallocation. */
int rbt_remove(rbt_t *rbt, void *key, freefunc_t freekey, freefunc_t freeitem);

/* Remove and return item associated with key. 
 * Return NULL if key not associated with item. 
 * Optional function-pointers for deallocation of key and item, pass NULL to avoid deallocation. */
void *rbt_pop(rbt_t *rbt, void *key, freefunc_t freekey);

/* Print rbt-structure using plot-library with string-representations of items using provided function-pointer. */
void rbt_print(rbt_t *rbt, strfunc_t strfunc);

/* Return n'th item starting from last item inserted. Unless rbt is sorted, then starting from lowest item.
 * '0' is first item and 'rbt_size() - 1' is last item. 
 * Return NULL if n is out of bounds. */
void *rbt_getitem(rbt_t *rbt, int n);

/* Sort iteration sequence for rbt. 
 * Should not be sorted after creation of iterator or in mid-iteration, if done so call rbt_resetiterator. */
void rbt_sort(rbt_t *rbt);


/* Iteration: */
typedef struct rbt_iterator rbt_iterator_t;

/* Return iterator over rbt. 
 * NOTE: When insertion into rbt is performed, iteration sequence is in reversed order related to insertion order.
 * Meaning first inserted item is last in iteration. 
 * Call 'rbt_sort()' before iterator-creation, or 'rbt_sort()' followed by 'rbt_resetiterator()'. */
rbt_iterator_t *rbt_createiterator(rbt_t *rbt);

/* Deallocates iterator. */
void rbt_destroyiterator(rbt_iterator_t *iterator);

/* Return 0 if iterator is exhausted, 1 otherwise. */
int rbt_hasnext(rbt_iterator_t *iterator);

/* Return next item in sequence of iteration, and iterate iterator. 
 * Return NULL if iterator exhausted. */
void *rbt_next(rbt_iterator_t *iterator);

/* Sets iterator to start of iteration. */
void rbt_resetiterator(rbt_iterator_t *iterator);

#endif