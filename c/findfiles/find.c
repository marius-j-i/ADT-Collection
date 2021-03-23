/* Author: Marius Ingebrigtsen */

#include <string.h>
#include <ctype.h>

#include "../common.h"
#include "index.h"


static index_t *make_indexer(const char *rootdir) {
	index_t	*index;
	char	**files, **words;
	int		numfiles, numwords, i;

	index = index_create();
	files = find_files(rootdir, &numfiles);

	for(i = 0; i < numfiles; i++) {
		words = tokenize_file(files[i], &numwords);

		index_addpath(index, files[i], words, numwords);
	}
	free(files);

	return index;
}

static char *make_expr(const char **words, int n) {
	char *expr, *tmp;

	expr = strdup(*words);
	if(expr == NULL) {
		fatal_error("Out of memory. ");
	}

	for(int i = 1; i < n; i++) {
		tmp = expr;
		expr = concatenate_strings(3, expr, " ", words[i]);
		free(tmp);
	}

	return expr;
}

/* Check for terminating word */
static int is_reserved_word(char *word) {
	if(strcasecmp(word, "ANDNOT") == 0)
		return 1;
	else if(strcasecmp(word, "AND") == 0)
		return 1;
	else if(strcasecmp(word, "OR") == 0)
		return 1;
	else if(strcmp(word, "(") == 0)
		return 1;
	else if(strcmp(word, ")") == 0)
		return 1;
	else
		return 0;
}

/* Check for terminating char */
static int is_reserved_char(char a) {
	if(isspace(a))
		return 1;

	switch (a) {
	case '(':
		return 1;
	case ')':
		return 1;
	default:
		return 0;
	}
}

static char *substring(char *start, char *end) {
	char *s = malloc(end-start+1);
	if (s == NULL) {
		fatal_error("out of memory");
		goto end;
	}

	strncpy(s, start, end-start);
	s[end-start] = 0;

end:
	return s;
}

/* Splits the query into a list of tokens */
static list_t *tokenize_query(char *query) {
	char *term;
	list_t *processed;
	
	processed = list_create((cmpfunc_t)strcmp);

	while(*query != '\0') {
		/* -Wall or -Wextra gives warnings. 'isspace' takes an int, not char. */
		if(isspace((int)*query)) {
			/* Ignore whitespace */
			query++;
			continue;
		}
		else if(*query == '(') {
			list_addlast(processed, strdup("("));
			query++;
		}
		else if(*query == ')') {
			list_addlast(processed, strdup(")"));
			query++;
		}
		else {
			char *s;
			/* Get length of term*/
			for(s=query;!is_reserved_char(*s) && *s != '\0'; s++);
			/* Copy term */
			term = substring(query, s);
			query = s;
			/* Add to list */
			list_addlast(processed, term);
		}
	}

	return processed;
}

/* Processes and tokenizes the query. Would normally include
 * stemming and stopword removal. */ 
static list_t *preprocess_query(char *query) {
	char *word, *c, *prev;
	list_t *tokens;
	list_t *processed;
	list_iterator_t *iter;

	/* Create tokens */
	tokens = tokenize_query(query);
	processed = list_create((cmpfunc_t)strcmp);
	prev = NULL;

	iter = list_createiterator(tokens);
	while(list_hasnext(iter)) {
		word = list_next(iter);

		/* Is a word */
		if(!is_reserved_word(word)) {

			/* Convert to lowercase */
			for (c = word; *c; c++)
				*c = tolower(*c);

			/* Adjacent words */
			if(prev != NULL && !is_reserved_word(prev))
				list_addlast(processed, strdup("OR"));
		}
		/* Add to processed tokens */
		list_addlast(processed, word);
		prev = word;
	}

	list_destroyiterator(iter);
	list_destroy(tokens, NULL);

	return processed;
}

static void find(index_t *index, list_t *query) {
	list_t *res;
	list_iterator_t *iter;
	query_result_t *r;
	char *errmsg;

	res = index_query(index, query, &errmsg);
	if(res == NULL) {
		printf("ERROR: %s\n", errmsg);
		return;
	}
	
	iter = list_createiterator(res);

	while( (r = list_next(iter)) ) {
		printf("File: %s, Score: %f \n", r->path, r->score);
		free(r->path);
		free(r);
	}

	list_destroyiterator(iter);
	list_destroy(res, NULL);
}

int main(int argc, const char **argv) {
	const char *rootdir;
	char *expr;
	index_t *index;
	list_t *query;

	if(argc < 3) {
		printf("Usage: %s <search-dir> <expression> \n",  *argv);
		return 0;
	}
	rootdir = argv[1];

	index = make_indexer(rootdir);

	expr = make_expr(argv+2, argc - 2);

	query = preprocess_query(expr);

	find(index, query);

	list_destroy(query, free);
	free(expr);
	index_destroy(index);

	return 0;
}
