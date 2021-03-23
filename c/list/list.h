/* Author: Marius Ingebrigtsen */
#ifndef __LIST_H_
#define __LIST_H_

#include "../common.h"


/* List Structure. */
typedef struct list list_t;
/* Function-pointer for freeing items in list. */
typedef void (*freefunc_t)(void *);
/* Function pointer for printing items in list. */
typedef void (*list_printfunc_t)(void *);


/* NOTE: 
 * All functions allocating memory handles errors interally by terminating program with call to 'fatal_error'-function defined in common.h. 
 * Therefore no return value is NULL unless otherwise defined in funtion definition. */

/* Creates a new, empty list that uses the given comparison function to compare elements.  The comparison function accepts two elements,
 * and returns -1 if the first is smaller than the second, 1 if the first is greater than the second, and 0 if the elements are equal.
 * Returns the new list. */
list_t *list_create(cmpfunc_t cmpfunc);

/* Changes comparison function to given function-pointer. */
void list_cmpfunc(list_t *list, cmpfunc_t cmpfunc);

/* Destroy list. Optional function-pointer for deallocating items in list. 
 * Pass NULL as argument if items are not to be deallocated. */
void list_destroy(list_t *list, freefunc_t freefunc);

/* Return number of items in list. */
int list_size(list_t *list);

/* Add item to start of list. */
void list_addfirst(list_t *list, void *item);

/* Add item to end of list. */
void list_addlast(list_t *list, void *item);

/* Remove item from list. Return item removed if successful, otherwise NULL is returned if item not in list. */
void *list_remove(list_t *list, void *item);

/* Remove and return first element of list. NULL returned if list is empty. */
void *list_popfirst(list_t *list);

/* Remove and return last element of list. NULL returned if list is empty. */
void *list_poplast(list_t *list);

/* Return 1 if item is in list, 0 otherwise. */
int list_contains(list_t *list, void *item);

/* Sort items in list using 'cmpfunc' from 'list_create()'. */
void list_sort(list_t *list);

/* Print list by use of function-pointer. */
void list_print(list_t *list, list_printfunc_t print);


/* List Iterator. */
typedef struct list_iterator list_iterator_t;

/* Create new iterator over list. */
list_iterator_t *list_createiterator(list_t *iterator);

/* Destroy iterator. */
void list_destroyiterator(list_iterator_t *iterator);

/* Return 1 if iterator is not exhausted, or 0 otherwise. */
int list_hasnext(list_iterator_t *iterator);

/* Return next item in iteration, and iterates iterator. 
 * NULL is returned if iterator is exhausted. */
void *list_next(list_iterator_t *iterator);

/* Return previous item in iteration, and reverses iterator. 
 * NULL is returned if iterator is exhausted. */
void *list_previous(list_iterator_t *iterator);

/* Return current item in iteration without iterating iterator. 
 * NULL is returned if iterator is exhausted. */
void *list_current(list_iterator_t *iterator);

/* Set iterator to start, as when created. */
void list_resetiterator(list_iterator_t *iterator);

/* Set iterator to end of list. */
void list_setiterator_end(list_iterator_t *iterator);

#endif
