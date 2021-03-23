#include "rbt.h"
#include "../plot.h"


typedef enum {
	RED, BLACK
} color_t;

typedef struct node node_t;
struct node {
	void	*key, *item;
	node_t	*left, *right, *next;
	color_t	color;
};

struct rbt {
	node_t		*root, *head;
	cmpfunc_t	cmpfunc;
	int			children;
};


/* RBT Create: */
rbt_t *rbt_create(cmpfunc_t cmpfunc) {
	rbt_t *rbt;

	rbt = calloc(1, sizeof(rbt_t));
	if(rbt == NULL) {
		fatal_error("Out of memory.\n");
	}
	rbt->cmpfunc	= cmpfunc;

	return rbt;
}

/* RBT Destroy: */
static void _rbt_destroy_keys_items(node_t *current, freefunc_t freekey, freefunc_t freeitem) {
	if(current == NULL) {
		return;
	}
	_rbt_destroy_keys_items(current->left, freekey, freeitem);
	_rbt_destroy_keys_items(current->right, freekey, freeitem);
	
	freekey(current->key);
	freeitem(current->item);
	free(current);
}

static void _rbt_destroy_keys(node_t *current, freefunc_t freekey) {
	if(current == NULL) {
		return;
	}
	_rbt_destroy_keys(current->left, freekey);
	_rbt_destroy_keys(current->right, freekey);
	
	freekey(current->key);
	free(current);
}

static void _rbt_destroy_items(node_t *current, freefunc_t freeitem) {
	if(current == NULL) {
		return;
	}
	_rbt_destroy_items(current->left, freeitem);
	_rbt_destroy_items(current->right, freeitem);
	
	freeitem(current->item);
	free(current);
}

static void _rbt_destroy(node_t *current) {
	if(current == NULL) {
		return;
	}
	_rbt_destroy(current->left);
	_rbt_destroy(current->right);
	free(current);
}

void rbt_destroy(rbt_t *rbt, freefunc_t freekey, freefunc_t freeitem) {
	if( (freekey != NULL) && (freeitem != NULL) ) {
		_rbt_destroy_keys_items(rbt->root, freekey, freeitem);
	} else if(freekey != NULL) {
		_rbt_destroy_keys(rbt->root, freekey);
	} else if(freeitem != NULL) {
		_rbt_destroy_items(rbt->root, freeitem);
	} else {
		_rbt_destroy(rbt->root);
	}
	free(rbt);
}

/* RBT Size: */
static void _rbt_depth(node_t *current, int *depth, int level) {
	if(current == NULL) {
		return;
	}
	*depth = (level > *depth) ? level : *depth;
	_rbt_depth(current->left, depth, level + 1);
	_rbt_depth(current->right, depth, level + 1);
}

int rbt_size(rbt_t *rbt, int size) {
	int depth;

	if(size) {
		return rbt->children;
	}
	depth = 0;

	_rbt_depth(rbt->root, &depth, 0);

	return depth;
}

/* RBT Insert: */
static inline node_t *node_create(void *key, void *item, node_t *next) {
	node_t *node;

	node = calloc(1, sizeof(node_t));
	if(node == NULL) {
		fatal_error("Out of memory.\n");
	}
	node->key	= key;
	node->item	= item;
	node->color	= RED;
	node->next	= next;

	return node;
}

static inline node_t *rotate_left(node_t *node) {
	node_t *child;

	child		= node->right;
	node->right	= child->left;
	child->left	= node;

	return child;
}

static inline node_t *rotate_right(node_t *node) {
	node_t *child;

	child		= node->left;
	node->left	= child->right;
	child->right= node;

	return child;
}

static node_t *_rbt_insert(node_t *current, void *key, void *item, node_t **head, cmpfunc_t cmpfunc, int fromright, int *added) {
	int cmp;

	if(current == NULL) {
		*head = node_create(key, item, *head);
		return *head;
	}
	else if( (current->left != NULL) && (current->right != NULL) && 
			 (current->left->color == RED) && (current->right->color == RED) ) {

		current->color = RED;
		current->left->color = current->right->color = BLACK;
	}
	cmp = cmpfunc(key, current->key);

	if(cmp < 0) {

		current->left = _rbt_insert(current->left, key, item, head, cmpfunc, 0, added);

		/* Left-Left Case: */
		if( (current->left->left != NULL) && 
			(current->left->color == RED) && 
			(current->left->left->color == RED) ) {

			current = rotate_right(current);
			current->color = BLACK;
			current->right->color = RED;
		}
		/* Right-Left Case: */
		else if((fromright) && 
				(current->color == RED) && 
				(current->left->color == RED) ) {
			
			current = rotate_right(current);
		}

	} else if(cmp > 0) {

		current->right = _rbt_insert(current->right, key, item, head, cmpfunc, 1, added);

		/* Right-Right Case: */
		if( (current->right->right != NULL) && 
			(current->right->color == RED) && 
			(current->right->right->color == RED) ) {

			current = rotate_left(current);
			current->color = BLACK;
			current->left->color = RED;
		}
		/* Left-Right Case: */
		else if((!fromright) && 
				(current->color == RED) && 
				(current->right->color == RED) ) {

			current = rotate_left(current);
		}

	} else {
		*added = 0;
		current->item = item;
	}
	return current;
}

int rbt_insert(rbt_t *rbt, void *key, void *item) {
	int added;

	added = 1;

	rbt->root = _rbt_insert(rbt->root, key, item, &rbt->head, rbt->cmpfunc, 0, &added);
	rbt->root->color = BLACK;

	if(added) {
		rbt->children++;
	}
	return added;
}

/* Red Black Tree; Remove & Pop: */
static void node_destroy(node_t *node, freefunc_t freekey, freefunc_t freeitem) {
	if(freekey != NULL) {
			freekey(node->key);
	}
	if(freeitem != NULL) {
		freeitem(node->item);
	}
	free(node);
}

static node_t *_percolate(node_t *current, int leftrotate, int fromright, freefunc_t freekey, freefunc_t freeitem) {
	node_t *tmp;

	/* Split 4-Node: */
	if( (current->left != NULL) && (current->right != NULL) && 
		(current->left->color == RED) && (current->right->color == RED) ){
		current->color = RED;
		current->left->color = current->right->color = BLACK;
	}

	/* Exit-Strategy: */
	else if(current->left == NULL) {	/* Above 'if'-statement cannot be true if any of these are also, therefore 'else if'. */
		tmp = current->right;
		node_destroy(current, freekey, freeitem);
		return tmp;
	}
	else if(current->right == NULL) {
		tmp = current->left;
		node_destroy(current, freekey, freeitem);
		return tmp;
	}

	/* Percolation / Trickle Down: */
	else if(leftrotate) {
		current = rotate_left(current);	/* Rotate removal-node down left. */
		current->left = _percolate(current->left, 0, fromright, freekey, freeitem);	/* Recursive follow of node with right-rotation next. */

		/* Left-Left Case: */	/* NOTE: Unsure whether or not percolation upwards will generate a leaf-node in stead of deleted node. */
		if( (current->left->left != NULL) && 
			(current->left->color == RED) && 
			(current->left->left->color == RED) ) {

			current = rotate_right(current);
			current->color = BLACK;
			current->right->color = RED;
		}
		/* Right-Left Case: */
		else if((fromright) && 
				(current->color == RED) && 
				(current->left->color == RED) ) {

			current = rotate_right(current);
		}
	}
	else {
		current = rotate_right(current);	/* Rotate removal-node down right. */
		current->right = _percolate(current->right, 1, fromright, freekey, freeitem);	/* Recursive follow of node with left-rotation next. */

		/* Right-Right Case: */
		if( (current->right->right != NULL) && 
			(current->right->color == RED) && 
			(current->right->right->color == RED) ) {

			current = rotate_left(current);
			current->color = BLACK;
			current->left->color = RED;
		}
		/* Left-Right Case: */
		else if((!fromright) && 
				(current->color == RED) && 
				(current->right->color == RED) ) {

			current = rotate_left(current);
		}
	}
	
	return current;
}

static node_t *_rbt_remove(node_t *current, void *key, cmpfunc_t cmpfunc, freefunc_t freekey, freefunc_t freeitem, void **item) {
	int cmp;

	if(current == NULL) {
		return NULL;
	}
	cmp = cmpfunc(key, current->key);

	if(cmp < 0) {
		current->left = _rbt_remove(current->left, key, cmpfunc, freekey, freeitem, item);
	} else if(cmp > 0) {
		current->right = _rbt_remove(current->right, key, cmpfunc, freekey, freeitem, item);
	} else {
		*item = current->item;
		current = _percolate(current, 0, 0, freekey, freeitem);
	}

	return current;
}

int rbt_remove(rbt_t *rbt, void *key, freefunc_t freekey, freefunc_t freeitem) {
	void *item;

	item = NULL;

	rbt->root = _rbt_remove(rbt->root, key, rbt->cmpfunc, freekey, freeitem, &item);
	if(rbt->root != NULL) {
		rbt->root->color = BLACK;
	}
	if(item != NULL) {
		rbt->children--;
	}

	return (item == NULL) ? 0 : 1;
}

void *rbt_pop(rbt_t *rbt, void *key, freefunc_t freekey) {
	void *item;

	item = NULL;

	rbt->root = _rbt_remove(rbt->root, key, rbt->cmpfunc, freekey, NULL, &item);
	if(rbt->root != NULL) {
		rbt->root->color = BLACK;
	}
	if(item != NULL) {
		rbt->children--;
	}

	return item;
}

/* RBT Search: */
static void *_rbt_search(node_t *current, void *key, cmpfunc_t cmpfunc) {
	int cmp;

	if(current == NULL) {
		return NULL;
	}
	cmp = cmpfunc(key, current->key);

	if(cmp < 0) {
		return _rbt_search(current->left, key, cmpfunc);
	} else if(cmp > 0) {
		return _rbt_search(current->right, key, cmpfunc);
	} else {
		return current->item;
	}
}

void *rbt_search(rbt_t *rbt, void *key) {
	return _rbt_search(rbt->root, key, rbt->cmpfunc);
}

/* RBT Print: */
static char *rbt_color(node_t *node) {
	char *color;

	if(node->color == RED) {
		color = "red";
	} else {
		color = "black";
	}
	return color;
}

static void _rbt_print(node_t *current, plot_t *plot, strfunc_t strfunc) {
	static long int NULL_ID = 2;
	char *color;

	if(current == NULL) {
		return;
	}
	color = rbt_color(current);

	if(current->left != NULL) {
		plot_addlink2(plot, current, current->left, strfunc(current->item), strfunc(current->left->item), color, rbt_color(current->left));
		
		_rbt_print(current->left, plot, strfunc);

	} else {
		plot_addlink2(plot, current, (void*)NULL_ID++, strfunc(current->item), "NULL", color, "white");
	}

	if(current->right != NULL) {
		plot_addlink2(plot, current, current->right, strfunc(current->item), strfunc(current->right->item), color, rbt_color(current->right));
		
		_rbt_print(current->right, plot, strfunc);
	
	} else {
		plot_addlink2(plot, current, (void*)NULL_ID++, strfunc(current->item), "NULL", color, "white");
	}
}

void rbt_print(rbt_t *rbt, strfunc_t strfunc) {
	plot_t *plot;

	plot = plot_create("rbt");

	_rbt_print(rbt->root, plot, strfunc);

	plot_doplot(plot);

	plot_destroy(plot);
}

/* RBT Get Item: */
static void *rbt_firstitem(node_t *current) {
	void *item;

	item = NULL;
	while(current != NULL) {
		item = current->item;
		current = current->left;
	}
	return item;
}

static void *rbt_lastitem(node_t *current) {
	void *item;

	item = NULL;
	while(current != NULL) {
		item = current->item;
		current = current->right;
	}
	return item;
}

static int issorted(node_t *head, cmpfunc_t cmpfunc) {
	node_t *current, *cmpnode;/*  ØKSJDNFOIGNWPITNGPJNRGLJWNTG PROBLEM*/

	current = head;

	while(current != NULL) {
		cmpnode = current->next;
		while(cmpnode != NULL) {
			if(cmpfunc(current->key, cmpnode->key) > 0) {
				return 0;
			}
			cmpnode = cmpnode->next;
		}
		current = current->next;
	}
	return 1;
}

static node_t *split(node_t *head) {
	node_t *slow, *fast, *ndhead;

	slow = head;
	fast = head->next;

	while( (fast != NULL) && (fast->next != NULL) ) {
		slow = slow->next;
		fast = fast->next->next;
	}

	ndhead = slow->next;
	slow->next = NULL;

	return ndhead;
}

static node_t *merge(node_t *a, node_t *b, cmpfunc_t cmpfunc) {
	node_t *head, *tail;

	if(cmpfunc(a->key, b->key) < 0) {
		head = a;
		a = a->next;
	} else {
		head = b;
		b = b->next;
	}
	tail = head;

	while( (a != NULL) && (b != NULL) ) {
		if(cmpfunc(a->key, b->key) < 0) {
			tail->next = a;
			a = a->next;
		} else {
			tail->next = b;
			b = b->next;
		}
		tail = tail->next;
	}

	if(a == NULL) {
		tail->next = b;
	} else {
		tail->next = a;
	}

	return head;
}

static node_t *_mergesort(node_t *head, cmpfunc_t cmpfunc) {
	node_t *ndhead;

	if( (head == NULL) || (head->next == NULL) ) {
		return head;
	}
	ndhead	= split(head);

	head	= _mergesort(head, cmpfunc);
	ndhead	= _mergesort(ndhead, cmpfunc);

	return merge(head, ndhead, cmpfunc);
}

void *rbt_getitem(rbt_t *rbt, int n) {
	node_t *current;

	if( (n < 0) || 
		(n >= rbt->children) || 
		(rbt->head == NULL) ) {

		return NULL;
	} else if(n == 0) {
		return rbt_firstitem(rbt->root);
	} else if(n == (rbt->children - 1)) {
		return rbt_lastitem(rbt->root);
	}
	current = rbt->head;

	for(int i = 0; i < n; i++) {
		current = current->next;
	}

	return current->item;
}

/* RBT Sort: */
void rbt_sort(rbt_t *rbt) {
	if( !issorted(rbt->head, rbt->cmpfunc) ) {
		rbt->head = _mergesort(rbt->head, rbt->cmpfunc);
	}
}


/* Iterator-structure: */
struct rbt_iterator {
	node_t *head, *current;
};

/* RBT Create Iterator: */
rbt_iterator_t *rbt_createiterator(rbt_t *rbt) {
	rbt_iterator_t *iterator;

	iterator = calloc(1, sizeof(rbt_iterator_t));
	if(iterator == NULL) {
		fatal_error("Out of memory.\n");
	}
	iterator->head = rbt->head;
	iterator->current = iterator->head;

	return iterator;
}

/* RBT Destroy Iterator: */
void rbt_destroyiterator(rbt_iterator_t *iterator) {
	free(iterator);
}

/* RTB Has Next: */
int rbt_hasnext(rbt_iterator_t *iterator) {
	if(iterator->current == NULL) {
		return 0;
	}
	return 1;
}

/* RBT Next: */
void *rbt_next(rbt_iterator_t *iterator) {
	void *item;

	if(iterator->current == NULL) {
		return NULL;
	}
	item = iterator->current->item;
	iterator->current = iterator->current->next;

	return item;
}

/* RBT Reset Iterator: */
void rbt_resetiterator(rbt_iterator_t *iterator) {
	iterator->current = iterator->head;
}