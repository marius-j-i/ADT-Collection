/* Author: Marius Ingebrigtsen */
#ifndef __COMMON_H_
#define __COMMON_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/* Comparison function-pointer. 
 * Return less than zero if first argument is lower than the second. 
 * Return greater than zero if first argument is greater than second. 
 * Return 0 if arguments are equal. */
typedef int (*cmpfunc_t)(void*, void*);

/* Hash-function. 
 * Return same hased value for same item every time. */
typedef unsigned long (*hashfunc_t)(void*);

/* Function-pointer for deallocation of items in ADTs. */
typedef void (*freefunc_t)(void*);

/* Function-pointer returning string representation of item put into structure. */
typedef char* (*strfunc_t)(void*);



/* NOTE: 
 * All functions using memory allocation handles errors interally with function-call to 'fatal_error()'. 
 * As a consequence, no function returning a pointer will return NULL unless otherwise specified in function-documentation. */

/* Print error message before terminating program. */
void fatal_error(char *exitmsg, ...);

/* Read every individual word and put allocated strings into allocated char*-array of char*'s. 
 * Last index of allocated char*-array will be NULL. */
char **tokenize_file(const char *file, int *n);

/* Deallocate words and array from call to 'tokenize_file'. */
void tokenize_destroy(char **words);

/* Return array of strings with NULL termination at end. 
 * Strings are paths to files in rootdir, and in subsequent directories below. 
 * 2nd argument is optional address to variable to place number of entries in return array. 
 * Pass NULL to opt not to assign value. 
 * Array and contents are allocated. */
char **find_files(const char *rootdir, int *n);

/* Deallocate paths and array allocated in 'find_files'. */
void find_files_destroy(char **files);

/* Allocate using calloc 'n' entries of 'size' for each. 
 * Performs termination check internally. 
 * Usage: char *str = (char*)new_array(10, sizeof(char)); */
void *new_array(int n, size_t size);

/* Allocate new integer with value. */
int *new_integer(int value);

/* Return word with length between 1 and argument. 
 * Contents are randomized with rand(). */
char *generate_randomized_word(int maxlength);

/* Return pointer to start of argument string. 
 * Swaps the characters around in reverse-order. */
char *str_reverse(char *str);

/* Return allocated string-representation of argument-number in argument-base representation. */
char *int_to_ascii(int num, int base);

/* Return new allocated string with contents copied from first to last string. */
char *concatenate_strings(int num, const char *first, ...);

/* Return allocated string of 'len' length with binary-representation of 'num' argument. 
 * Return NULL if given length is 0. 
 * Example Usage: 
 * '''
 * float *f; *f = 1.1; 
 * char *bin = strbin((unsigned long long*)f, sizeof(float)*8);
 * printf("Binary-rep of %f = \'%s\'. \n", f, bin); free(bin);
 * '''
 */
char *strbin(unsigned long long *num, int len);

#endif