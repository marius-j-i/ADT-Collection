/* Author: Marius Ingebrigtsen */
/* Set-implementation is a Red-Black Tree. */
#include "set.h"


typedef enum color color_t;
enum color {
	BLACK, RED
};

typedef struct node node_t;
struct node {
	node_t	*left, *right, *next;
	color_t	color;
	void	*item;
};

struct set {
	node_t		*root, *head;
	int			children;
	cmpfunc_t	cmpfunc;
};


/* Set Create: */
set_t *set_create(cmpfunc_t cmpfunc) {
	set_t *set;

	set = malloc(sizeof(set_t));
	if(set == NULL) {
		fatal_error("Out of memory.\n");
	}
	set->root = set->head = NULL;
	set->children = 0;
	set->cmpfunc = cmpfunc;
	return set;
}

/* Set Destroy: */
static void _set_destroy(node_t *current, freefunc_t freefunc) {
	if(current == NULL) {
		return;
	}
	_set_destroy(current->left, freefunc);
	_set_destroy(current->right, freefunc);
	if(freefunc != NULL) {
		freefunc(current->item);
	}
	free(current);
}

void set_destroy(set_t *set, freefunc_t freefunc) {
	_set_destroy(set->root, freefunc);
	free(set);
}

/* Set Depth: */
static void _set_depth(node_t *current, int current_level, int *level) {
	if(current == NULL) {
		return;
	}*level = (*level > current_level) ? *level : current_level;
	_set_depth(current->left, current_level + 1, level);
	_set_depth(current->right, current_level + 1, level);
}

/* Debugging function. Returns deepest recursion depth for tree. */
int set_depth(set_t *set) {
	int level = 0;
	_set_depth(set->root, 0, &level);
	return level;
}

/* Set Size: */
int set_size(set_t *set) {
	return set->children;
}

/* Set Add: */
static inline node_t *node_create(void *item, node_t *previous, set_t *set) {
	node_t *node;

	node = (node_t*)malloc(sizeof(node_t));
	if(node == NULL) {
		fatal_error("Out of memory.\n");
	}
	node->color		= RED;
	node->left		= node->right = NULL;
	node->item		= item;
	if(previous == NULL) {	/* If node is inserted in root. */
		set->head = node;
		node->next = NULL;
	} else {				/* Insert node in-between. */
		node->next = previous->next;
		previous->next = node;
	}
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

static node_t *_set_insert(node_t *current, node_t *previous, set_t *set, void *item, cmpfunc_t cmpfunc, int from_right, int *added) {
	int cmp;

	if(current == NULL) {
		return node_create(item, previous, set);
	}
	/* Color children black if both are red, and current node red. */
	else if( (current->left != NULL) && (current->right != NULL) && 
			 (current->left->color == RED) && (current->right->color == RED) ) {
		current->color = RED;
		current->left->color = BLACK;
		current->right->color = BLACK;
	}

	cmp = cmpfunc(item, current->item);
	
	if(cmp < 0) {
		current->left	= _set_insert(current->left, current, set, item, cmpfunc, 0, added);

		/* 1: RED-LEFT-LEFT CASE */
		if( (current->left->left != NULL) && (current->left->left->color == RED) && (current->left->color == RED) ) {

			current					= rotate_right(current);
			current->color			= BLACK;
			current->right->color	= RED;
		}
		/* 2: RED-RIGHT-LEFT CASE --- ROTATE RIGHT ON CURRENT */
		else if( (from_right == 1) && (current->color == RED) && (current->left->color == RED) ) {

			current = rotate_right(current);
		}
	}
	else if(cmp > 0) {
		current->right	= _set_insert(current->right, current, set, item, cmpfunc, 1, added);

		/* 3: RED-RIGHT-RIGHT CASE */
		if( (current->right->right != NULL) && (current->right->right->color == RED) && (current->right->color == RED) ) {
			
			current					= rotate_left(current);
			current->color			= BLACK;
			current->left->color	= RED;
		}
		/* 4: RED-LEFT-RIGHT CASE */
		else if( (from_right == 0) && (current->color == RED) && (current->right->color == RED)) {
			
			current = rotate_left(current);
		}
	}
	else {
		*added = 0;
	}
	return current;
}

int set_add(set_t *set, void *item) {
	int added;

	added = 1;
	set->root = _set_insert(set->root, set->root, set, item, set->cmpfunc, 0, &added);	/* _set_insert() already works like set_contains() in that no duplicate will be inserted. An int indicates whether a node was created or not. */
	set->root->color = BLACK;
	if(added) {
		set->children++;
	}
	return added;
}

/* Set Contains: */
static void _set_contains(node_t *current, void *item, cmpfunc_t cmpfunc, int *contains) {
	if(current == NULL) {
		return;
	}int cmp = cmpfunc(item, current->item);
	if(cmp < 0) {
		_set_contains(current->left, item, cmpfunc, contains);
	}else if(cmp > 0) {
		_set_contains(current->right, item, cmpfunc, contains);
	}else {
		*contains = 1;
	}return;
}

int set_contains(set_t *set, void *item) {
	int contains = 0;
	_set_contains(set->root, item, set->cmpfunc, &contains);
	return contains;
}

/* Set Union: */
static void _set_union(node_t *current, set_t *Union, copyfunc_t copyfunc) {
	void *item;

	if(current == NULL) {
		return;
	}else if( (copyfunc != NULL) && (!set_contains(Union, current->item)) ) {	/* Check if item not in set. */
		item = copyfunc(current->item);	/* Only copy item if item is assuredly not in set (no way to free item if already added). */
	}else {
		item = current->item;
	}
	set_add(Union, item);
	_set_union(current->left, Union, copyfunc);
	_set_union(current->right, Union, copyfunc);
}

set_t *set_union(set_t *a, set_t *b, copyfunc_t copyfunc) {
	set_t *Union;	/* 'union' namespace is taken by C. */

	Union = set_copy(a, copyfunc);
	_set_union(b->root, Union, copyfunc);
	return Union;
}

/* Set Intersection: */
static void _set_intersection(node_t *current, set_t *b, set_t *intersect, copyfunc_t copyfunc) {
	void *item;

	if(current == NULL) {
		return;
	}
	else if(set_contains(b, current->item)) {
		if(copyfunc != NULL) {
			item = copyfunc(current->item);
		}else {
			item = current->item;
		}
		set_add(intersect, item);
	}
	_set_intersection(current->left, b, intersect, copyfunc);
	_set_intersection(current->right, b, intersect, copyfunc);
}

set_t *set_intersection(set_t *a, set_t *b, copyfunc_t copyfunc) {
	set_t *intersect;

	intersect = set_create(a->cmpfunc);
	_set_intersection(a->root, b, intersect, copyfunc);
	return intersect;
}

/* Set Difference: */
static void _set_difference(node_t *current, set_t *b, set_t *difference, copyfunc_t copyfunc) {
	void *item;

	if(current == NULL) {
		return;
	}
	else if(!set_contains(b, current->item)) {
		if(copyfunc != NULL) {
			item = copyfunc(current->item);
		}else {
			item = current->item;
		}
		set_add(difference, item);
	}
	_set_difference(current->left, b, difference, copyfunc);
	_set_difference(current->right, b, difference, copyfunc);
}

set_t *set_difference(set_t *a, set_t *b, copyfunc_t copyfunc) {
	set_t *difference;

	difference = set_create(a->cmpfunc);
	_set_difference(a->root, b, difference, copyfunc);
	return difference;
}

/* Set Copy: */
/* Insertion works like an unbalanced binary searching tree. */
static node_t *_set_add_unbalanced(node_t *current, node_t *previous, set_t *set, void *item, cmpfunc_t cmpfunc, color_t color) {
	node_t	*node;
	int		cmp;

	if(current == NULL) {
		node = node_create(item, previous, set);
		node->color = color;
		return node;
	}
	cmp = cmpfunc(item, current->item);
	if(cmp < 0) {
		current->left = _set_add_unbalanced(current->left, current, set, item, cmpfunc, color);
	}else if(cmp > 0) {
		current->right = _set_add_unbalanced(current->right, current, set, item, cmpfunc, color);
	}
	return current;
}

/* Adding function for copying without right- or left-rotation. */
static inline void set_add_unbalanced(set_t *set, void *item, color_t color) {
	set->root = _set_add_unbalanced(set->root, set->root, set, item, set->cmpfunc, color);
	set->children++;
}

/* Make a blue-copy of tree without right- or left-rotation. */
static void _set_copy(node_t *current, set_t *copy, copyfunc_t copyfunc) {
	void *item;

	if(current == NULL) {
		return;
	}else if(copyfunc != NULL) {	/* If copy-function provided; copy item. */
		item = copyfunc(current->item);
	}else {							/* If no copy-function; create copy-set with same items as argument set. */
		item = current->item;
	}
	set_add_unbalanced(copy, item, current->color);
	_set_copy(current->left, copy, copyfunc);
	_set_copy(current->right, copy, copyfunc);
}

set_t *set_copy(set_t *set, copyfunc_t copyfunc) {
	set_t *copy;

	copy = set_create(set->cmpfunc);
	_set_copy(set->root, copy, copyfunc);
	return copy;
}

/* Set Sort: */
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
	node_t	*head, *tail;

	if(cmpfunc(a->item, b->item) < 0) {
		head = tail = a;
		a = a->next;
	} else {
		head = tail = b;
		b = b->next;
	}

	while( (a != NULL) && (b != NULL) ) {
		if(cmpfunc(a->item, b->item) < 0) {
			tail->next = a;
			a = a->next;
		} else {
			tail->next = b;
			b = b->next;
		}
		tail = tail->next;
	}

	if(a != NULL) {
		tail->next = a;
	} else if(b != NULL) {
		tail->next = b;
	}
	return head;
}

static node_t *_set_mergesort(node_t *head, cmpfunc_t cmpfunc) {
	node_t *ndhead;

	if(head == NULL || head->next == NULL) {
		return head;
	}
	ndhead	= split(head);
	head	= _set_mergesort(head, cmpfunc);
	ndhead	= _set_mergesort(ndhead, cmpfunc);
	return merge(head, ndhead, cmpfunc);
}

void set_sort(set_t *set) {
	set->head = _set_mergesort(set->head, set->cmpfunc);
}


/* Set Iterator: */
struct set_iterator {
	set_t	*set;
	node_t	*next;
};

set_iterator_t *set_createiterator(set_t *set) {
	set_iterator_t *iterator;

	iterator = (set_iterator_t*)malloc(sizeof(set_iterator_t));
	if(iterator == NULL) {
		fatal_error("Out of memory.\n");
	}
	iterator->set = set;
	iterator->next = set->head;

	return iterator;
}

void set_destroyiterator(set_iterator_t *iterator) {
	free(iterator);
}

int set_hasnext(set_iterator_t *iterator) {
	if(iterator->next == NULL) {
		return 0;
	}
	return 1;
}

void *set_next(set_iterator_t *iterator) {
	void *item;

	if(iterator->next == NULL) {
		return NULL;
	}
	item = iterator->next->item;
	iterator->next = iterator->next->next;

	return item;
}

void set_resetiterator(set_iterator_t *iterator) {
	iterator->next = iterator->set->head;
}