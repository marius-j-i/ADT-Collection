/* Author: Marius Ingebrigtsen */
#include "query.h"

#include <string.h>


/*	Query BNF Language: 

	query	::=	andterm
			|	andterm "ANDNOT" query

	andterm	::=	orterm
			|	orterm "AND" andterm

	orterm	::=	term
			|	term "OR" orterm

	term	::=	"(" query ")"
			|	<word>					*/

typedef struct parsenode parsenode_t;
struct parsenode {			/* Nodes for AST-structure. */
	operator_type	operator;
	parsenode_t		*left, *right;
	char			*word;
};

typedef struct context {	/* Struct for constructing AST from a list. */
	list_iterator_t	*iterator;
	char			*word;
} context_t;

struct query {				/* Implementation of an Abstract Syntax Tree. */
	parsenode_t		*root;
	operatorfunc_t	operation;
};

/* Declaration of parse-functions. */
static parsenode_t *parse_query(context_t *context, char **errmsg, int *error);
static parsenode_t *parse_andterm(context_t *context, char **errmsg, int *error);
static parsenode_t *parse_orterm(context_t *context, char **errmsg, int *error);
static parsenode_t *parse_term(context_t *context, char **errmsg, int *error);


query_t *query_create(operatorfunc_t operation) {
	query_t *parse;

	parse = (query_t*)malloc(sizeof(query_t));
	if(parse == NULL) {
		fatal_error("Out of memory.\n");
	}
	parse->root		= NULL;
	parse->operation= operation;
	return parse;
}

static parsenode_t *node_create(operator_type operator, char *word, parsenode_t *left, parsenode_t *right) {
	parsenode_t *node;
	node = (parsenode_t*)malloc(sizeof(parsenode_t));
	if(node == NULL) {
		fatal_error("Out of memory.\n");
	}
	node->operator	= operator;
	node->left		= left;
	node->right		= right;
	node->word		= word;
	return node;
}

/*	query	::=	andterm
			|	andterm "ANDNOT" query	*/
static parsenode_t *parse_query(context_t *context, char **errmsg, int *error) {
	parsenode_t *andterm, *query;

	andterm = parse_andterm(context, errmsg, error);
	if(*error) {
		return andterm;
	}
	else if( (context->word != NULL) && 					/* Example Seg-Fault Case:		query == "graph". */
			 (strcasecmp(context->word, "ANDNOT") == 0) ) {

		if(!list_hasnext(context->iterator)) {				/* Example Syntax-Error Case:	query == "... graph ANDNOT". */
			*error = 1;
			*errmsg = "\'ANDNOT\'-operator without follow-up.";
		}

		context->word = list_next(context->iterator);
		query = parse_query(context, errmsg, error);

		return node_create(ANDNOT, NULL, andterm, query);
	}
	return andterm;
}

/*	andterm	::=	orterm
			|	orterm "AND" andterm	*/
static parsenode_t *parse_andterm(context_t *context, char **errmsg, int *error) {
	parsenode_t *orterm, *andterm;
	
	orterm = parse_orterm(context, errmsg, error);
	if(*error) {
		return orterm;
	}
	else if( (context->word != NULL) && 					/* Example Seg-Fault Case:		query == "graph". */
			 (strcasecmp(context->word, "AND") == 0) ) {

		if(!list_hasnext(context->iterator)) {				/* Example Syntax-Error Case:	query == "... graph AND". */
			*error = 1;
			*errmsg = "\'AND\'-operator without follow-up.";
		}
		context->word = list_next(context->iterator);
		andterm = parse_andterm(context, errmsg, error);

		return node_create(AND, NULL, orterm, andterm);
	}
	return orterm;
}

/*	orterm	::=	term
			|	term "OR" orterm		*/
static parsenode_t *parse_orterm(context_t *context, char **errmsg, int *error) {
	parsenode_t *term, *orterm;

	term = parse_term(context, errmsg, error);
	if(*error) {
		return term;
	}
	else if( (context->word != NULL) && 					/* Example Seg-Fault Case:		query == "graph". */
			 (strcasecmp(context->word, "OR") == 0) ) {

		if(!list_hasnext(context->iterator)) {				/* Example Syntax-Error Case:	query == "... graph OR". */
			*error = 1;
			*errmsg = "\'OR\'-operator without follow-up.";
		}
		context->word = list_next(context->iterator);
		orterm = parse_term(context, errmsg, error);

		return node_create(OR, NULL, term, orterm);
	}
	return term;
}

/*	 term	::=	"(" query ")"
			|	<word>					*/
static parsenode_t *parse_term(context_t *context, char **errmsg, int *error) {
	parsenode_t *query;
	char *tmpword;

	tmpword = context->word;
	context->word = list_next(context->iterator);

	if( (tmpword != NULL) && 						/* Example Seg-Fault Case:	query == "... (graph OR". */
		(*tmpword == '(') ) {						/* Dereference variable without seg-fault. */

		query = parse_query(context, errmsg, error);

		if( ((context->word != NULL) && (*context->word != ')')) || /* If no corresponding end-parenthesis. */
			(context->word == NULL) ) {								/* Or if abrupt end of query. */

			*error = 1;
			*errmsg = "Missing end-parenthesis.";	/* Invalid query. Preserve syntax-tree for later recursive deallocation. */
		}
		context->word = list_next(context->iterator);

		return query;

	}else {
		return node_create(STRING, tmpword, NULL, NULL);
	}
}

int query_parse(query_t *parse, list_t *query, char **errmsg) {
	context_t *context;
	int error;

	if(list_size(query) == 0) {
		error = 1;
		*errmsg = "Query empty.";
		return error;
	}

	context = (context_t*)malloc(sizeof(context_t));	/* Compiler warning says 'context' may be uninitialized. */
	if(context == NULL) {
		fatal_error("Out of memory.\n");
	}
	context->iterator	= list_createiterator(query);
	context->word		= list_next(context->iterator);
	error				= 0;

	parse->root = parse_query(context, errmsg, &error);
	
	list_destroyiterator(context->iterator);
	free(context);
	
	return error;
}

static void _query_destroy(parsenode_t *current) {
	if(current == NULL) {
		return;
	}
	_query_destroy(current->left);
	_query_destroy(current->right);
	free(current);
}

void query_destroy(query_t *parse) {
	_query_destroy(parse->root);/* If parse->root is NULL it will return and free parse. */
	free(parse);				/* Whether or not query has a tree, free query. */
}

static void _query_print(parsenode_t *current) {
	if(current == NULL) {
		return;
	}
	_query_print(current->left);
	_query_print(current->right);
	if(current->operator == ANDNOT) {
		printf("Operator: \'ANDNOT\'. \n");
	}
	else if(current->operator == AND) {
		printf("Operator: \'AND\'. \n");
	}
	else if(current->operator == OR) {
		printf("Operator: \'OR\'. \n");
	}
	else if(current->operator == STRING) {
		printf("Word: \'%s\'. \n", current->word);
	}
}

void query_print(query_t *parse) {
	_query_print(parse->root);
}

/* Recursively find terms in constructed tree and call on operator functions. */
static void *parse_evaluate(parsenode_t *current, operatorfunc_t operation, char **errmsg) {
	void *item, *item_left, *item_right;

	switch(current->operator) {
		case ANDNOT: {
			item_left = parse_evaluate(current->left, operation, errmsg);
			if(item_left == NULL) {
				goto error;
			}
			item_right = parse_evaluate(current->right, operation, errmsg);
			if(item_right == NULL) {
				goto error;
			}

			return operation(current->operator, item_left, item_right);
		}
		case AND: {
			item_left = parse_evaluate(current->left, operation, errmsg);
			if(item_left == NULL) {
				goto error;
			}
			item_right = parse_evaluate(current->right, operation, errmsg);
			if(item_right == NULL) {
				goto error;
			}

			return operation(current->operator, item_left, item_right);
		}
		case OR: {
			item_left = parse_evaluate(current->left, operation, errmsg);
			if(item_left == NULL) {
				goto error;
			}
			item_right = parse_evaluate(current->right, operation, errmsg);
			if(item_right == NULL) {
				goto error;
			}

			return operation(current->operator, item_left, item_right);
		}
		case STRING: {
			item = operation(current->operator, current->word, NULL);

			if(item == NULL) {
				*errmsg = "No match for query.";
				goto error;
			}

			return item;
		}
		default: {
			*errmsg = "Unsupported query.";
			goto error;
		}
	}

error:
	return NULL;
}

void *query_evaluate(query_t *parse, char **errmsg) {
	void *result;

	if(parse->root == NULL) {
		goto error;
	}

	result = parse_evaluate(parse->root, parse->operation, errmsg);

	if(result == NULL) {	/* Error message already appended at this error. */
		goto error;
	}

	return result;

error:
	if(parse->root == NULL) {
		*errmsg = "Empty query.";
	}
	return NULL;
}