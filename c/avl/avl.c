 /* Author: Marius Ingebrigtsen */

#include "avl.h"
#include "../plot.h"

#define MAX(a, b) (a > b) ? a : b

typedef struct node node_t;
struct node {
	void	*key, *item;
	node_t	*left, *right, *next;
	int		subtree;
};

struct avl {
	node_t		*root, *head;
	int			children;
	cmpfunc_t	cmpfunc;
};


/* AVL Create: */
avl_t *avl_create(cmpfunc_t cmpfunc) {
	avl_t *avl;

	avl = calloc(1, sizeof(avl_t));
	if(avl == NULL) {
		fatal_error("Out of memory.");
	}
	avl->cmpfunc	= cmpfunc;

	return avl;
}

/* AVL Destroy: */
static void _avl_destroy_key_item(node_t *current, freefunc_t freekey, freefunc_t freeitem) {
	if(current == NULL) {
		return;
	}
	_avl_destroy_key_item(current->left, freekey, freeitem);
	_avl_destroy_key_item(current->right, freekey, freeitem);
	freekey(current->key);
	freeitem(current->item);
	free(current);
}

static void _avl_destroy_key(node_t *current, freefunc_t freekey) {
	if(current == NULL) {
		return;
	}
	_avl_destroy_key(current->left, freekey);
	_avl_destroy_key(current->right, freekey);
	freekey(current->key);
	free(current);
}

static void _avl_destroy_item(node_t *current, freefunc_t freeitem) {
	if(current == NULL) {
		return;
	}
	_avl_destroy_item(current->left, freeitem);
	_avl_destroy_item(current->right, freeitem);
	freeitem(current->item);
	free(current);
}

static void _avl_destroy(node_t *current) {
	if(current == NULL) {
		return;
	}
	_avl_destroy(current->left);
	_avl_destroy(current->right);
	free(current);
}

void avl_destroy(avl_t *avl, freefunc_t freekey, freefunc_t freeitem) {
	if( (freekey != NULL) && (freeitem != NULL) ) {
		_avl_destroy_key_item(avl->root, freekey, freeitem);
	} else if(freekey != NULL) {
		_avl_destroy_key(avl->root, freekey);
	} else if(freeitem != NULL) {
		_avl_destroy_item(avl->root, freeitem);
	} else {
		_avl_destroy(avl->root);
	}
	free(avl);
}

/* AVL Size: */
int avl_size(avl_t *avl) {
	return avl->children;
}

/* AVL Insertion: */
static inline node_t *node_create(void *key, void *item, node_t *head) {
	node_t *node;

	node = malloc(sizeof(node_t));
	if(node == NULL) {
		fatal_error("Out of memory.");
	}
	node->key		= key;
	node->item		= item;
	node->left		= node->right = NULL;
	node->subtree	= 1;

	node->next	= head;

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

static inline int subtree_height(node_t *node) {
	if(node == NULL) {
		return 0;
	}
	return node->subtree;
}

static node_t *_avl_insert(node_t *current, cmpfunc_t cmpfunc, void *key, void *item, node_t **head, int *inserted) {
	int cmp, balance;

	if(current == NULL) {
		*head = node_create(key, item, *head);
		return *head;
	}
	cmp = cmpfunc(key, current->key);

	if(cmp < 0) {
		current->left = _avl_insert(current->left, cmpfunc, key, item, head, inserted);

		current->subtree = MAX( subtree_height(current->left), subtree_height(current->right) ) + 1;

		balance = subtree_height(current->left) - subtree_height(current->right);

		if(balance > 1) {
			cmp = cmpfunc(key, current->left->key);

			/* LEFT-LEFT CASE: */
			if(cmp < 0) {
				current = rotate_right(current);
				current->right->subtree = MAX( subtree_height(current->right->left), subtree_height(current->right->right) ) + 1;
			}
			/* LEFT-RIGHT CASE: */
			else if(cmp > 0) {
				current->left = rotate_left(current->left);
				current->left->subtree = MAX( subtree_height(current->left->left), subtree_height(current->left->right) ) + 1;
				current->left->left->subtree = MAX( subtree_height(current->left->left->left), subtree_height(current->left->left->right) ) + 1;
			}
		}
	}
	else if(cmp > 0) {
		current->right = _avl_insert(current->right, cmpfunc, key, item, head, inserted);

		current->subtree = MAX( subtree_height(current->left), subtree_height(current->right) ) + 1;

		balance = subtree_height(current->left) - subtree_height(current->right);

		if(balance < -1) {
			cmp = cmpfunc(key, current->right->key);

			/* RIGHT-RIGHT CASE: */
			if(cmp > 0) {
				current = rotate_left(current);
				current->left->subtree = MAX( subtree_height(current->left->left), subtree_height(current->left->right) ) + 1;
			}
			/* RIGHT-LEFT CASE: */
			else if(cmp < 0) {
				current->right = rotate_right(current->right);
				current->right->subtree = MAX( subtree_height(current->right->left), subtree_height(current->right->right) ) + 1;
				current->right->right->subtree = MAX( subtree_height(current->right->right->left), subtree_height(current->right->right->right) ) + 1;
			}
		}
	}
	else {
		*inserted = 0;
		current->item = item;
		return current;
	}
	current->subtree = MAX( subtree_height(current->left), subtree_height(current->right) ) + 1;

	return current;
}

int avl_insert(avl_t *avl, void *key, void *item) {
	int inserted;

	inserted = 1;

	avl->root = _avl_insert(avl->root, avl->cmpfunc, key, item, &avl->head, &inserted);

	if(inserted) {
		avl->children++;
	}

	return inserted;
}

/* AVL Search: */
static void *_avl_search(node_t *current, cmpfunc_t cmpfunc, void *key) {
	int cmp;

	if(current == NULL) {
		return NULL;
	}
	cmp = cmpfunc(key, current->key);

	if(cmp < 0) {
		return _avl_search(current->left, cmpfunc, key);
	}
	else if(cmp > 0) {
		return _avl_search(current->right, cmpfunc, key);
	}
	else {
		return current->item;
	}
	return NULL;
}

void *avl_search(avl_t *avl, void *key) {
	return _avl_search(avl->root, avl->cmpfunc, key);
}

/* AVL Sort: */
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
	ndhead = split(head);

	head = _mergesort(head, cmpfunc);
	ndhead = _mergesort(ndhead, cmpfunc);

	return merge(head, ndhead, cmpfunc);
}

void avl_sort(avl_t *avl) {
	if(avl->children < 2) {
		return;
	}
	avl->head = _mergesort(avl->head, avl->cmpfunc);
}

/* AVL Print: */
static void _avl_print(node_t *current, plot_t *plot, strfunc_t strfunc) {
	static long int NULL_ID = 2;

	if(current == NULL) {
		return;
	}

	if(current->left != NULL) {
		plot_addlink2(plot, current, current->left, strfunc(current->item), strfunc(current->left->item), "white", "white");

		_avl_print(current->left, plot, strfunc);
	} else {
		plot_addlink2(plot, current, (void*)NULL_ID++, strfunc(current->item), "NULL", "white", "white");
	}

	if(current->right != NULL) {
		plot_addlink2(plot, current, current->right, strfunc(current->item), strfunc(current->right->item), "white", "white");

		_avl_print(current->right, plot, strfunc);
	} else {
		plot_addlink2(plot, current, (void*)NULL_ID++, strfunc(current->item), "NULL", "white", "white");
	}
}

void avl_print(avl_t *avl, char *pdfname, strfunc_t strfunc) {
	plot_t *plot;

	plot = plot_create(pdfname);

	_avl_print(avl->root, plot, strfunc);

	plot_doplot(plot);

	plot_destroy(plot);
}


/* AVL Iteration */
/* Iterator Structure: */
struct avl_iterator {
	avl_t	*avl;
	node_t	*current;
};

/* AVL Create Iterator: */
avl_iterator_t *avl_createiterator(avl_t *avl) {
	avl_iterator_t *iterator;

	iterator = malloc(sizeof(avl_iterator_t));
	if(iterator == NULL) {
		fatal_error("Out of memory.");
	}
	iterator->avl = avl;
	iterator->current = avl->head;

	return iterator;
}

/* AVL Destroy Iterator: */
void avl_destroyiterator(avl_iterator_t *iterator) {
	free(iterator);
}

/* AVL Has Next: */
int avl_hasnext(avl_iterator_t *iterator) {
	if(iterator->current == NULL) {
		return 0;
	}
	return 1;
}

/* AVL Next: */
void *avl_next(avl_iterator_t *iterator) {
	void *item;

	// printf("%p\n", iterator);

	if(iterator->current == NULL) {
		return NULL;
	}

	item = iterator->current->item;

	iterator->current = iterator->current->next;

	return item;
}

/* AVL Reset Iterator: */
void avl_resetiterator(avl_iterator_t *iterator) {
	iterator->current = iterator->avl->head;
}