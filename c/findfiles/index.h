/* Author: Marius Ingebrigtsen */
#ifndef __INDEX_H_
#define __INDEX_H_

#include "../common.h"
#include "../list/list.h"


/* Index structure definition. */
typedef struct index index_t;

typedef struct query_result {
	char	*path;	/* File-path. */
	double	score;	/* Score for successful query into 'path'. */
} query_result_t;


/* NOTE: 
 * All functions allocating memory handles errors interally by terminating 
 * program with call to 'fatal_error'-function defined in common.h. 
 * Therefore no return value is NULL unless otherwise defined in funtion 
 * definition. */

/* Return newly created index. */
index_t *index_create();

/* Destroy index and all interally allocated memory not otherwise 
 * specified outside index-ADT responsibility. */
void index_destroy(index_t *index);

/* Add 'path' to 'index', and index strings in 'words'-list under 'path'. 
 * If 'path' is allready indexed 'index_addpath() will not index that path. 
 * Return 1 if indexed, otherwise return 0. 
 * NOTE: 
 * Deallocation of 'path' and string-contents in 'words'-list is 
 * responsibility of 'index_addpath()' */
int index_addpath(index_t *index, char *path, char **words, int n);

/* Debugging function. 
 * Prints to terminal strings in 'words'-list indexed to file-paths. */
void index_print(index_t *index, list_t *words);

/* Perform query on 'index'. 
 * If successful, return-value is list of 'query_result_t*'-items. 
 * If query triggers an error, an error-message is put into 'errmsg' 
 * and return-value is NULL. 
 * Each item in return-list contains file-path to matching query, 
 * and score pertaining to how relevant file is in relation to all 
 * query_result_t's. 
 * Each 'query_result_t' in return-list will have memory allocated 
 * for structure itself, as well as each 'path' in list-items. 
 * These will not be deallocated in 'index_destroy()' and is 
 * responsibility of ADT-user. 
 */
list_t *index_query(index_t *index, list_t *query, char **errmsg);

#endif