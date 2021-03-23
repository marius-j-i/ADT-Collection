/* Author: Marius Ingebrigtsen */
/* NOTE: Recursive descent in a splay tree may resolve into a linked list and cause stackoverflow. */

#include "splay.h"
#include "../plot.h"


typedef struct node node_t;
struct node {
	void	*key, *item;
	node_t	*left, *right, *next;
};

typedef struct splay {
	node_t		*root, *head;
	int			children;
	cmpfunc_t	cmpfunc;
} splay_t;


/* Splay Create: */
splay_t *splay_create(cmpfunc_t cmpfunc) {
	splay_t *splay;

	splay = calloc(1, sizeof(splay_t));
	if(splay == NULL) {
		fatal_error("Out of memory.");
	}
	splay->cmpfunc = cmpfunc;

	return splay;
}

/* Splay Destroy: */
static void _splay_destroy_key_item(node_t *current, freefunc_t freekey, freefunc_t freeitem) {
	if(current == NULL) {
		return;
	}
	printf("%d\n", *(int*)current->item);
	_splay_destroy_key_item(current->left, freekey, freeitem);
	_splay_destroy_key_item(current->right, freekey, freeitem);
	
	freekey(current->key);
	freeitem(current->item);
	free(current);
}

static void _splay_destroy_key(node_t *current, freefunc_t freekey) {
	if(current == NULL) {
		return;
	}
	_splay_destroy_key(current->left, freekey);
	_splay_destroy_key(current->right, freekey);

	freekey(current->key);
	free(current);
}

static void _splay_destroy_item(node_t *current, freefunc_t freeitem) {
	if(current == NULL) {
		return;
	}
	_splay_destroy_item(current->left, freeitem);
	_splay_destroy_item(current->right, freeitem);

	freeitem(current->item);
	free(current);
}

static void _splay_destroy(node_t *current) {
	if(current == NULL) {
		return;
	}
	_splay_destroy(current->left);
	_splay_destroy(current->right);
	free(current);
}

void splay_destroy(splay_t *splay, freefunc_t freekey, freefunc_t freeitem) {
	if(freekey && freeitem) {
		_splay_destroy_key_item(splay->root, freekey, freeitem);
	} else if(freekey) {
		_splay_destroy_key(splay->root, freekey);
	} else if(freeitem) {
		_splay_destroy_item(splay->root, freekey);
	} else {
		_splay_destroy(splay->root);
	}
	free(splay);
}

/* Splay Depth: */
static void _splay_depth(node_t *current, int *depth, int level) {
	
	if(current == NULL) {
		return;
	}
	*depth = (level > *depth) ? level : *depth;
	_splay_depth(current->left, depth, level + 1);
	_splay_depth(current->right, depth, level + 1);
}

int splay_depth(splay_t *splay) {
	int depth;

	depth = 0;

	_splay_depth(splay->root, &depth, 0);

	return depth;
}

/* Splay Size: */
int splay_size(splay_t *splay) {
	return splay->children;
}

/* Splay Insert: */
static node_t *rotate_left(node_t *current) {
	node_t *child;

	child = current->right;
	current->right = child->left;
	child->left = current;

	return child;
}

static node_t *rotate_right(node_t *current) {
	node_t *child;

	child = current->left;
	child->right = current->left;
	child->right = current;

	return child;
}

static node_t *node_create(void *key, void *item, node_t *left, node_t *right, node_t *head) {
	node_t *node;

	node = malloc(sizeof(node_t));
	if(node == NULL) {
		fatal_error("Out of memory.\n");
	}
	node->key	= key;
	node->item	= item;
	node->left	= left;
	node->right	= right;
	node->next	= head;

	return node;
}

static node_t *_splay_insert(node_t *current, node_t **head, cmpfunc_t cmpfunc, void *key, void *item, int *inserted) {
	int cmp;

	if(current == NULL) {
		*head = node_create(key, item, NULL, NULL, *head);
		return *head;
	}
	cmp = cmpfunc(key, current->key);

	if(cmp < 0) {

		if(current->left == NULL) {
			current->left = node_create(key, item, NULL, NULL, *head);
			*head = current->left;
			current = rotate_right(current);

			return current;
		}
		cmp = cmpfunc(key, current->left->key);

		if(cmp < 0) {		/* Left-Left Case: */

			current->left->left = _splay_insert(current->left->left, head, cmpfunc, key, item, inserted);

			current = rotate_right(current);
			current = rotate_right(current);
		}
		else if(cmp > 0) {	/* Left-Right Case: */

			current->left->right = _splay_insert(current->left->right, head, cmpfunc, key, item, inserted);

			current->left = rotate_left(current->left);
			current = rotate_right(current);
		}
		else {	/* Key exists. */
			current->left->item = item;
			*inserted = 0;
		}
	}
	else if(cmp > 0) {

		if(current->right == NULL) {
			current->right = node_create(key, item, NULL, NULL, *head);
			*head = current->right;
			current = rotate_left(current);

			return current;
		}
		cmp = cmpfunc(key, current->right->key);

		if(cmp < 0) {		/* Right-Left Case: */

			current->right->left = _splay_insert(current->right->left, head, cmpfunc, key, item, inserted);

			current->right = rotate_right(current->right);
			current = rotate_left(current);
		}
		else if(cmp > 0) {	/* Right-Right Case: */

			current->right->right = _splay_insert(current->right->right, head, cmpfunc, key, item, inserted);

			current = rotate_left(current);
			current = rotate_left(current);
		}
		else {				/* Key exists. */
			current->item = item;
			*inserted = 0;
		}
	}
	else {	/* Key exists. */
		current->item = item;
		*inserted = 0;
	}

	return current;
}

int splay_insert(splay_t *splay, void *key, void *item) {
	int inserted;

	inserted = 1;

	splay->root = _splay_insert(splay->root, &splay->head, splay->cmpfunc, key, item, &inserted);

	if(inserted) {
		splay->children++;
	}

	return inserted;
}

/* Splay Search: */
static void _splay_search(node_t *current, void *key, cmpfunc_t cmpfunc, void **item) {
	int cmp;

	if(current == NULL) {
		return;
	}
	cmp = cmpfunc(key, current->key);

	if(cmp < 0) {

		if(current->left == NULL) {
			return;
		}
		cmp = cmpfunc(key, current->left->key);

		if(cmp < 0) {		/* Left-Left Case. */
			_splay_search(current->left->left, key, cmpfunc, item);

			if(*item != NULL) {
				current = rotate_right(current);
				current = rotate_right(current);
			}

		} else if(cmp > 0) {/* Left-Right Case. */
			_splay_search(current->left->right, key, cmpfunc, item);

			if(*item != NULL) {
				current->left = rotate_left(current->left);
				current = rotate_right(current);
			}

		} else {			/* Left is target. */
			*item = current->item;
		}

	} else if(cmp > 0) {

		if(current->right == NULL) {
			return;
		}
		cmp = cmpfunc(key, current->right->key);

		if(cmp < 0) {		/* Right-Left Case. */
			_splay_search(current->right->left, key, cmpfunc, item);

			if(*item != NULL) {
				current->right = rotate_right(current->right);
				current = rotate_left(current);
			}

		} else if(cmp > 0) {/* Right-Right Case. */
			_splay_search(current->right->right, key, cmpfunc, item);

			if(*item != NULL) {
				current = rotate_left(current);
				current = rotate_left(current);
			}

		} else {			/* Right is target. */
			*item = current->item;
		}

	} else {
		*item = current->item;
	}
}

void *splay_search(splay_t *splay, void *key) {
	void *item;

	item = NULL;
	_splay_search(splay->root, key, splay->cmpfunc, &item);

	return item;
}

/* Splay Print: */
static void _splay_print(node_t *current, plot_t *plot, strfunc_t strfunc) {
	static long int NULL_ID = 2;

	if(current == NULL) {
		return;
	}
	else if(current->left != NULL) {

		plot_addlink2(plot, current, current->left, strfunc(current->item), strfunc(current->left->item), "white", "white");

		_splay_print(current->left, plot, strfunc);

	} else {
		plot_addlink2(plot, current, (void*)NULL_ID++, strfunc(current->item), "NULL", "white", "white");
	}

	if(current->right != NULL) {

		plot_addlink2(plot, current, current->right, strfunc(current->item), strfunc(current->right->item), "white", "white");

		_splay_print(current->right, plot, strfunc);

	} else {
		plot_addlink2(plot, current, (void*)NULL_ID++, strfunc(current->item), "NULL", "white", "white");
	}
}

void splay_print(splay_t *splay, strfunc_t strfunc) {
	plot_t *plot;

	plot = plot_create("Splay");
	if(plot == NULL) {
		fatal_error("Out of memory.");
	}

	_splay_print(splay->root, plot, strfunc);
	
	plot_doplot(plot);
}