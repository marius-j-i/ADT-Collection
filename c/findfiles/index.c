/* Author: Marius Ingebrigtsen */
#include "index.h"
#include "map.h"
#include "set.h"
#include "lookup3.h"
#include "query.h"

#include <string.h>
#include <math.h>


/* Index Structure: */
struct index {
	map_t	*map_path;		/* Map with word-keys mapped to sets containing paths to files that contain that word. */
	map_t	*map_counter;	/* Map with path-keys mapped to 'file_counter'-structs, with one 'file_counter' for each path. */
	int		corpus;			/* Counter for number of file-entries in index. */
};

typedef struct file_counter {/* Struct for calculating tf-idf. */
	map_t	*word_occur;	/* Map with word-keys mapped to numerical counters for how many times that word occurs in file. */
	int		file_size;		/* Number of words in file. */
} file_counter_t;


/* Index Create: */
index_t *index_create() {
	index_t *index;
	
	index = (index_t*)malloc(sizeof(index_t));
	if(index == NULL) {
		fatal_error("Out of memory.\n");
	}
	index->map_path		= map_create( (cmpfunc_t)strcasecmp, lookup3);
	index->map_counter	= map_create( (cmpfunc_t)strcasecmp, lookup3);
	index->corpus		= 0;
	return index;
}

/* Index Destroy: */
/* Deallocation-function, used in index_destroy to free sets in 'map_path'.  */
static void free_sets(void *item) {
	set_t *set_paths;

	set_paths = (set_t*)item;
	/* Items in 'set_paths' are; 'char *path': free(). */
	set_destroy(set_paths, free);
}

/* Deallocation-function, used in index_destroy to free file_counter_t in 'map_counter'. */
static void free_file_counters(void *item) {
	file_counter_t *file_counter;

	file_counter = (file_counter_t*)item;
	/* Keys and Values in 'word_occur' are; 'char *word': free(), and 'int *count': free(). */
	map_destroy(file_counter->word_occur, free, free);
	free(file_counter);
}

void index_destroy(index_t *index) {
	/* Keys and Values in 'map_path' are; 'char *word': free(), and 'set_t*': free_sets(). */
	map_destroy(index->map_path, free, free_sets);				/* Note: Items in sets are 'path'-strings allocated in this ADT. */
	/* Keys and Values in 'map_counter' are; 'char *path'; free(), and 'file_counter_t *file_counter': free_file_counters(). */
	map_destroy(index->map_counter, free, free_file_counters);
	free(index);
}

/* Index Addpath: */
static void index_map_inverse(map_t *map_path, char *word, char *path, int *corpus) {
	set_t *set_path;

	if( (set_path = map_get(map_path, word)) != NULL ) {/* If 'word'-key registered to pre-existing set of paths containing 'word'.  */
	}													/* Retrieve set mapped to 'word' and update 'word' to also refer to 'path' from function argument. */
	else {												/* If key not in map, then index-map contain no set of files with 'word' in it. */
		set_path = set_create( (cmpfunc_t)strcasecmp );
		map_put(map_path, strdup(word), set_path);		/* Map 'word'-duplication to paths. */
	}

	path = strdup(path);								/* Create duplicate file-path. */
	if( !set_add(set_path, path) ) {					/* Add duplicate of file-path to previously existing, or new, set of paths mapped to 'word'. */
		free(path);										/* In case of 'word' occuring in file more than once, then 'path' will not be added second time or later. So free allocated dupclicate. */
	}
	*corpus += 1;										/* Increment index-table count. */
}

static void create_file_counter(map_t *map_counter, char *word, char *path, int file_size) {
	file_counter_t	*file_counter;
	int				*count;

	file_counter = (file_counter_t*)malloc(sizeof(file_counter_t));
	if(file_counter == NULL) {
		fatal_error("Out of memory.\n");
	}

	/* Map of Word Occurrence: */
	file_counter->word_occur = map_create( (cmpfunc_t)strcasecmp, lookup3);	/* Map for word-keys mapped to frequency-counts of words in file-path. */
	file_counter->file_size = file_size;									/* Record file size for this path. */

	count = new_integer(1);													/* Allocate count for 'word'. */

	map_put(file_counter->word_occur, strdup(word), count);					/* Map duplicate of 'word' to 'count' of frequency in 'path'. */

	map_put(map_counter, strdup(path), file_counter);						/* Map duplicate of 'path' to 'file_counter' for this file. */
}

static void update_file_counter(file_counter_t *file_counter, char *word) {
	int *count;

	if( (count = map_get(file_counter->word_occur, word)) ) {	/* If file-counter for file-path have counted 'word' already. */
		*count += 1;											/* Retrieve counter of frequency for 'word', and increment frequency count. */
	}
	else {														/* If file-counter for file-path have not counted 'word' before. */
		count = (int*)malloc(sizeof(int));						/* Create new count. */
		if(count == NULL) {
			fatal_error("Out of memory.\n");
		}
		*count = 1;
		map_put(file_counter->word_occur, strdup(word), count);	/* Map duplicate of 'word' to new 'count'. */
	}
}

static void index_file_counter(map_t *map_counter, char *word, char *path, int file_size) {
	file_counter_t *file_counter;

	if( (file_counter = map_get(map_counter, path)) != NULL ) {	/* If file-path have file-counter. */
		update_file_counter(file_counter, word);				/* Update file-counter. */
	}
	else {														/* If file-path have no file-counter. */
		create_file_counter(map_counter, word, path, file_size);/* Create new counter for file. */
	}
}

int index_addpath(index_t *index, char *path, char **words, int n) {
	/* NOTE:
	 * This function foregoes some function-calls to 'map_haskey()' at expense of readability, 
	 * but for drastic increase in performance by directly attempting to 'map_get()' and reviewing the results. */

	int i;

	/* File is already indexed if there is counter for it. */
	if(map_haskey(index->map_counter, path)) {	
		goto done;
	}

	for(i = 0; i < n; i++) {
		/* Map 'word' to all file-paths containing 'word'. */
		index_map_inverse(index->map_path, words[i], path, &index->corpus);

		/* Update or create file-counter for file. */
		index_file_counter(index->map_counter, words[i], path, n);

		/* Free word, as per index-ADT's responsibility. */
		free(words[i]);
	}
	free(words);
	/* Free path, as per index-ADT's responsibility. */
	free(path);

	return 1;

done:
	for(i = 0; i < n; i++) {
		free(words[i]);
	}
	free(words);
	free(path);

	return 0;
}

/* Index Print: */
void index_print(index_t *index, list_t *words) {
	list_iterator_t	*list_iter;
	set_t			*pathset;
	set_iterator_t	*set_iter;
	char			*word, *path;

	list_iter = list_createiterator(words);
	
	while( (word = list_next(list_iter)) ) {

		pathset = map_get(index->map_path, word);
		
		if(pathset == NULL) {
			printf("\'%s\' gave no result. \n", word);
			continue;
		}

		set_iter = set_createiterator(pathset);
		while( (path = set_next(set_iter)) ) {
			printf("\'%s\' is indexed to \t\'%s\'. \n", word, path);
		}
		set_destroyiterator(set_iter);
	}
	list_destroyiterator(list_iter);
}

/* Index Query: */

/* Global hashmap and linkedlist for operator-functions reference. */
map_t	*index_map;		/* Map set to index->map for operator-function. */
list_t	*delete_list;	/* List for keeping reference to set unions, intersections and differences for later deallocation. */

static void *copyfunc(void *item) {
	char *path;

	path = item;
	return strdup(path);
}

static set_t *andnot(set_t *one, set_t *two) {
	set_t *set;

	/* Perform operation and use 'strdup' to copy set-content. */
	set = set_difference(one, two, copyfunc);
	list_addlast(delete_list, set);
	return set;
}

static set_t *and(set_t *one, set_t *two) {
	set_t *set;

	/* Perform operation and use 'strdup' to copy set-content. */
	set = set_intersection(one, two, copyfunc);
	list_addlast(delete_list, set);
	return set;
}

static set_t *or(set_t *one, set_t *two) {
	set_t *set;

	/* Perform operation and use 'strdup' to copy set-content. */
	set = set_union(one, two, copyfunc);
	list_addlast(delete_list, set);
	return set;
}

static set_t *string(char *word) {
	if(word == NULL) {
		return NULL;
	}
	return map_get(index_map, word);
}

static void *operation(operator_type operator, void *item_one, void *item_two) {
	switch(operator) {
		case ANDNOT: {
			return andnot(item_one, item_two);
		}
		case AND: {
			return and(item_one, item_two);
		}
		case OR: {
			return or(item_one, item_two);
		}
		case STRING: {
			return string(item_one);
		}
		default: {
			return NULL;
		}
	}
}

static void setlist_delete(list_t *setlist) {	/* Function for destroying all sets and global list. */
	list_iterator_t *iterator;
	set_t *set;

	iterator = list_createiterator(setlist);

	while( (set = list_next(iterator)) ) {
		/* Items in 'set' are; 'char *path': free(). 
		 * Duplicates of paths in sets mapped in index->map_path with word-keys. */
		set_destroy(set, free);
	}
	list_destroyiterator(iterator);
	list_destroy(setlist, NULL);
}

static double tf(index_t *index, list_t *query, char *path) {
	list_iterator_t	*iterator;
	file_counter_t	*file_counter;
	char			*word;
	int				*count;
	double			term_freq = 0;

	file_counter = (file_counter_t*)map_get(index->map_counter, path);
	iterator = list_createiterator(query);

	while( (word = list_next(iterator)) ) {
		if( (*word == '(') || (*word == ')')	|| 
			(strcasecmp(word, "ANDNOT")	== 0)	|| 
			(strcasecmp(word, "AND")	== 0)	|| 
			(strcasecmp(word, "OR")		== 0)	) {
			continue;
		}
		else if( (count = map_get(file_counter->word_occur, word)) != NULL ) {
			term_freq += *(int*)count;
		}
	}

	list_destroyiterator(iterator);

	return term_freq / file_counter->file_size;	/* tf(t,d) = sum of (term frequency in file / sum of (all term frequencies in file) ). */
}

static double idf(double index_corpus, double evaluation_size) {
	return log10(index_corpus / evaluation_size);	/* idf(t,D) = log( number corpus-docs / number query docs ). */
}

static double tf_idf(index_t *index, list_t *query, char *path, int evaluation_size) {
	return tf(index, query, path) * idf(index->corpus, evaluation_size);
}

/* Cmpfunc for sorting query_result_t-list by score, starting higest and decending. */
static int compare_score(query_result_t *a, query_result_t *b) {
	if(a->score == b->score) {
		return 0;
	}
	/* Return scheme is inverted in relation to function-pointer definition of cmpfunc_t in common.h. */
	return (a->score < b->score) ? 1 : -1;
}

static list_t *process_query(index_t *index, set_t *evaluation, list_t *query) {
	list_t			*query_result;
	set_iterator_t	*iterator;
	query_result_t	*result;
	char			*path;

	query_result = list_create( (cmpfunc_t)compare_score );

	iterator = set_createiterator(evaluation);

	while( (path = set_next(iterator)) ) {

		result = (query_result_t*)malloc(sizeof(query_result_t));
		if(result == NULL) {
			fatal_error("Out of memory.\n");
		}

		/* Allocate 'path'-duplicate so setlist_delete() does not free and lose 'result->path'-reference. */
		result->path	= strdup(path);
		result->score	= tf_idf(index, query, path, set_size(evaluation));

		list_addlast(query_result, result);
	}
	set_destroyiterator(iterator);

	return query_result;
}

list_t *index_query(index_t *index, list_t *query, char **errmsg) {
	query_t	*parser;
	set_t	*evaluation;
	list_t	*query_result;
	int		query_error;

	*errmsg = "\0";	/* Set message to print nothing if no specific has occurred. */

	parser = query_create( (operatorfunc_t)operation );

	query_error	= 0;
	query_error	= query_parse(parser, query, errmsg);

	if(query_error) {
		goto error;
	}

	delete_list	= list_create( (cmpfunc_t)strcasecmp );		/* Create list for operator-functions to append all resulting sets. */
	index_map	= index->map_path;							/* Assign path-map to global map for operator-function reference. */

	evaluation	= query_evaluate(parser, errmsg);			/* Evaluate parsed query and recieve resulting set. This set also exists in 'delete_list'. */

	if( (evaluation == NULL) ) { 
		goto error;
	}

	query_result = process_query(index, evaluation, query);	/* Create list of query_result_t with paths and their calculated tf-idf's. */

	list_sort(query_result);

	query_destroy(parser);
	setlist_delete(delete_list);

	return query_result;

error:
	if(query_error) {
		query_destroy(parser);
	}
	else if(evaluation == NULL) {
		query_destroy(parser);
		setlist_delete(delete_list);
	}
	return NULL;
}