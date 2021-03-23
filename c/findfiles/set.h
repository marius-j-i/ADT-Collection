/* Author: Marius Ingebrigtsen */
#ifndef __SET_H_
#define __SET_H_

#include "../common.h"
#include "../list/list.h"

/* Set Structure. */
typedef struct set set_t;
/* Optional function pointer for deallocation of items in set. */
typedef void (*freefunc_t)(void *);
/* Optional function pointer for allocation of items in set-operations. */
typedef void* (*copyfunc_t)(void *);


/* NOTE: 
 * All functions allocating memory handles errors interally by terminating program with call to 'fatal_error'-function defined in common.h. 
 * Therefore no return value is NULL unless otherwise defined in funtion definition. */

/* Creates new set with compare function to compare items in set. */
set_t *set_create(cmpfunc_t cmpfunc);

/* Destroys set. If items are not to be destroyed provide function-pointer with NULL-argument. */
void set_destroy(set_t *set, freefunc_t free_item);

/* Returns cardinality of set. */
int set_size(set_t *set);

/* Add item into set. If item is in set, no add will be performed. Returns 1 if added, 0 otherwise. */
int set_add(set_t *set, void *item);

/* Returns 1 if item is in set, 0 otherwise. */
int set_contains(set_t *set, void *item);

/* Returns new set with all elements in 'a' and 'b'. 
 * NOTE: Items in set-result will be allocated separately from previous sets, using given function pointer. 
 * If sets are to have the same item reference provide function-pointer with NULL-argument. */
set_t *set_union(set_t *a, set_t *b, copyfunc_t copyfunc);

/* Returns new set with elements both in 'a' and in 'b'. 
 * NOTE: Items in set-result will be allocated separately from previous sets, using given function pointer. 
 * If sets are to have the same item reference provide function-pointer with NULL-argument. */
set_t *set_intersection(set_t *a, set_t *b, copyfunc_t copyfunc);

/* Returns new set with elements in 'a' and not in 'b'. 
 * NOTE: Items in set-result will be allocated separately from previous sets, using given function pointer. 
 * If sets are to have the same item reference provide function-pointer with NULL-argument. */
set_t *set_difference(set_t *a, set_t *b, copyfunc_t copyfunc);

/* Returns new set-duplicate of given set. 
 * NOTE: Items in set-result will be allocated separately from previous sets, using given function pointer. 
 * If sets are to have the same item reference provide function-pointer with NULL-argument. */
set_t *set_copy(set_t *set, copyfunc_t copyfunc);

/* Sort set for iteration. */
void set_sort(set_t *set);


/* Iterator Structure. */
typedef struct set_iterator set_iterator_t;

/* Creates new iterator for set. */
set_iterator_t *set_createiterator(set_t *set);

/* Destroys iterator. */
void set_destroyiterator(set_iterator_t *iterator);

/* Returns 0 if iterator is at end of the set. Returns 1 otherwise. */
int set_hasnext(set_iterator_t *iterator);

/* Returns next element in ordered sequence. Returns NULL when at end of iteration. */
void *set_next(set_iterator_t *iterator);

/* Resets iterator to start of sorted iteration. */
void set_resetiterator(set_iterator_t *iterator);

#endif