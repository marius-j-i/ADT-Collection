/* Author: Marius Ingebrigsten */
#ifndef __QUERY_H_
#define __QUERY_H_

#include "../common.h"
#include "../list/list.h"


/*	Query BNF Language: 

	query	::=	andterm
			|	andterm "ANDNOT" query

	andterm	::=	orterm
			|	orterm "AND" andterm

	orterm	::=	term
			|	term "OR" orterm

	term	::=	"(" query ")"
			|	<word>					*/

typedef struct query query_t;

/* Enums are used by query ADT to indicate which operation is being performed. */
typedef enum {
	ANDNOT, AND, OR, STRING /* 'WORD' namespace (in capital) is already taken :´( */
} operator_type;

/* Function-pointer takes an enum and two void-pointers. 
 * Enum is indication for which operation is being performed when called. 
 * If 'operator_type' is STRING, then third argument will be NULL and second will be 'char *'. 
 * If 'operator_type' is not STRING, then both void-arguments will be the return value of two different calls with STRING. 
 * Return pointer of user-specified item to perform operations on and ultimately recieve from 'query_evaluate'-function. 
 * If return value is NULL, query_evalutation will conclude with "No match for query."-error. */
typedef void* (*operatorfunc_t)(operator_type, void*, void*);


/* NOTE: 
 * All functions allocating memory handles errors interally by terminating program with call to 'fatal_error'-function defined in common.h. 
 * Therefore no return value is NULL unless otherwise defined in funtion definition. */

/* Create query with function-pointer for performing 'ANDNOT'-, 'AND'-, 'OR'- and 'STRING'-operations for this BNF-language. */
query_t *query_create(operatorfunc_t operation);

/* Parse query-list and put result in 'parse'. Return 1 if query-list is invalid and puts error-message in 'errmsg'. Return 0 otherwise. */
int query_parse(query_t *parse, list_t *query, char **errmsg);

/* Destroy query, freeing allocated memory. */
void query_destroy(query_t *parse);

/* Debugging function. Prints structure to terminal. */
void query_print(query_t *parse);

/* Evaluates 'parse'-query and uses its operator-function to compute the return pointer. 
 * Return NULL if query-evaluation gives invalid outcome and puts error in errmsg. */
void *query_evaluate(query_t *parse, char **errmsg);

#endif