/* Author: Marius Ingebrigtsen */
#ifndef __MAP_H_
#define __MAP_H_

#include "../common.h"

/* Map Structure. */
typedef struct map map_t;

/* NOTE: 
 * All functions allocating memory handles errors interally by terminating program with call to 'fatal_error'-function defined in common.h. 
 * Therefore no return value is NULL unless otherwise defined in funtion definition. */

/* Create map with function pointers to comparison function between keys added in map, and hash-function to hash keys. */
map_t *map_create(cmpfunc_t cmpfunc, hashfunc_t hashfunc);

/* Destroy map. 
 * Second and third arguments are optional function-pointers for user-defined functions to deallocate keys and values used in map. 
 * Pass 'NULL' if keys and/or values are not to be destroyed. */
void map_destroy(map_t *map, freefunc_t freekey, freefunc_t freevalue);

/* Return number of entries into map. */
int map_size(map_t *map);

/* Map 'key' to 'value'. If key already in map then new value will overwrite previous. 
 * Return 1 if put in map. Return 0 if value associated with key was overwritten. */
int map_put(map_t *map, void *key, void *value);

/* Return 1 if map contain key, 0 otherwise. */
int map_haskey(map_t *map, void *key);

/* Return value mapped to key in maps. NULL returned if key not mapped to any value. */
void *map_get(map_t *map, void *key);

#endif