/* Author: Marius Ingebrigtsen */
#include "list.h"


typedef struct node node_t;
struct node {
	node_t	*next, *previous;
	void 	*item;
};

struct list {
	node_t		*head, *tail;
	int			num_items;
	cmpfunc_t	cmpfunc;
};


/* List Create: */
list_t *list_create(cmpfunc_t cmpfunc) {
	list_t *list = malloc(sizeof(list_t));
	if(list == NULL) {
		fatal_error("Out of memory.");
	}list->head = list->tail = NULL;
	list->num_items = 0;
	list->cmpfunc = cmpfunc;
	return list;
}

/* List Compare Funciton: */
void list_cmpfunc(list_t *list, cmpfunc_t cmpfunc) {
	list->cmpfunc = cmpfunc;
}

/* List Destroy: */
static void free_nodes_items(node_t *current, freefunc_t freefunc) {
	node_t *tmp;

	while(current != NULL) {
		tmp = current;
		current = current->next;
		freefunc(tmp->item);
		free(tmp);
	}
}

static void free_nodes(node_t *current) {
	node_t *tmp;

	while(current != NULL) {
		tmp = current;
		current = current->next;
		free(tmp);
	}
}

void list_destroy(list_t *list, freefunc_t freefunc) {
	if(list->head == NULL) {
		free(list);
		return;
	} else if(freefunc != NULL) {
		free_nodes_items(list->head, freefunc);
	} else {
		free_nodes(list->head);
	}
	free(list);
}

/* List Size: */
int list_size(list_t *list) {
	return list->num_items;
}

/* List Add-First & Add-Last: */
static inline node_t *create_node(void *item) {
	node_t *node = (node_t*)malloc(sizeof(node_t));
	if(node == NULL) {
		fatal_error("Out of memory.");
	}node->item	= item;
	node->next	= node->previous = NULL;
	return node;
}

void list_addfirst(list_t *list, void *item) {
	node_t *node	= create_node(item);
	node->next		= list->head;
	list->head		= node;
	if(list->num_items > 0)
		node->next->previous = node;	/* Patch together the node ahead without seg fault. */
	else
		list->tail = node;				/* List was empty, and the above patch would've been seg fault. Head and tail point to the only item in the list. */
	list->num_items++;
}

void list_addlast(list_t *list, void *item) {
	node_t *node	= create_node(item);
	node->previous	= list->tail;
	list->tail		= node;
	if(list->num_items > 0)				/* Then there's a node behind the new last node and then patch won't be dereference of NULL. */
		node->previous->next = node;
	else
		list->head = node;				/* Then the list is empty, and head and tail should point to the only node in the list. */
	list->num_items++;
}

/* List Remove, Pop-First & Pop-Last: */
void *list_remove(list_t *list, void *item) {
	node_t	*current;

	if( (list->head != NULL) && 
		(list->cmpfunc(list->head->item, item) == 0) ) {	/* If first element in list is 'item'. */
		return list_popfirst(list);
	}
	else if( (list->tail != NULL) && 
			 (list->cmpfunc(list->head->item, item) == 0) ) {	/* If last element in list is 'item'. */
		return list_poplast(list);
	}
	else if(list->num_items <= 2) {	/* If 'head' and 'tail' are all items in list. */
		return NULL;
	}
	current = list->head->next;	/* 'head' is not NULL or 'item', and 'head->next' is not 'tail' because 'num_items' > 2. */
	
	while(current->next != NULL) {	/* Loop terminates when 'current' is 'tail'. */
		if(list->cmpfunc(item, current->item) == 0) {
			
			current->previous->next = current->next;		/* At this point 'current' is assured to not be either head or tail. */
			current->next->previous = current->previous;	/* So previous and next will never be NULL. */

			free(current);

			list->num_items--;

			return item;
		}
		current = current->next;
	}
	return NULL;
}

void *list_popfirst(list_t *list) {
	if(list->head == NULL) {
		return NULL;
	}void *item = list->head->item;
	node_t *tmp = list->head;
	list->head = list->head->next;
	free(tmp);
	list->num_items--;
	if(list->head == NULL) {
		list->tail = NULL;
	}return item;
}

void *list_poplast(list_t *list) {
	if(list->tail == NULL) {
		return NULL;
	}void *item = list->tail->item;
	node_t *tmp = list->tail;
	list->tail = list->tail->previous;
	free(tmp);
	list->num_items--;
	if(list->tail == NULL) {
		list->head = NULL;
	}return item;
}

/* List Contains: */
int list_contains(list_t *list, void *item) {
	if(list->num_items == 0) {
		return 0;
	}node_t *current = list->head;
	while(current != NULL) {
		if(list->cmpfunc(current->item, item) == 0) {	// Returns -1 if first argument is smaller, 1 if first argument is greater, and 0 if the arguments are equal. 
			return 1;
		}current = current->next;
	}return 0;
}

/* List Print: */
void list_print(list_t *list, list_printfunc_t printfunc) {
	node_t *current;

	if(list->head == NULL) {
		return;
	}
	current = list->head;

	while(current != NULL) {
		printfunc(current->item);
		current = current->next;
	}
}

/* List Sort: */
static node_t *split(node_t *head) {
	node_t *slow, *fast, *ndhead;
	slow = head;
	fast = head->next;
	while(fast != NULL && fast->next != NULL) {
		slow = slow->next;
		fast = fast->next->next;
	}ndhead = slow->next;
	slow->next = NULL;
	return ndhead;
}

static node_t *merge(node_t *A, node_t *B, cmpfunc_t cmpfunc) {
	node_t *head, *tail;
	if(cmpfunc(A->item, B->item) < 0) {
		head = tail = A;
		A = A->next;
	}else {
		head = tail = B;
		B = B->next;
	}while(A != NULL && B != NULL) {
		if(cmpfunc(A->item, B->item) < 0) {
			tail->next = A;
			tail = A;
			A = A->next;
		}else {
			tail->next = B;
			tail = B;
			B = B->next;
		}
	}if(A != NULL) {
		tail->next = A;
	}else if(B != NULL) {
		tail->next = B;
	}return head;
}

static node_t *_mergesort(node_t *head, cmpfunc_t cmpfunc) {
	if(head == NULL || head->next == NULL) {
		return head;
	}node_t *ndhead = split(head);
	head			= _mergesort(head, cmpfunc);
	ndhead			= _mergesort(ndhead, cmpfunc);
	return merge(head, ndhead, cmpfunc);
}

void list_sort(list_t *list) {
	list->head = _mergesort(list->head, list->cmpfunc);
}


/* List Iterator: */
struct list_iterator {
	list_t 	*list;
	node_t	*current;
};

list_iterator_t *list_createiterator(list_t *list) {
	list_iterator_t *iterator;

	iterator = malloc(sizeof(list_iterator_t));
	if(iterator == NULL) {
		fatal_error("Out of memory.");
	}
	iterator->current	= list->head;
	iterator->list		= list;

	return iterator;
}

void list_destroyiterator(list_iterator_t *iterator) {
	free(iterator);
}

int list_hasnext(list_iterator_t *iterator) {
	if(iterator->current == NULL) {
		return 0;
	}
	return 1;
}

void *list_next(list_iterator_t *iterator) {
	void *item;

	if(iterator->current == NULL) {
		return NULL;
	}
	item = iterator->current->item;
	iterator->current = iterator->current->next;
	
	return item;
}

void *list_previous(list_iterator_t *iterator) {
	void *item;

	if(iterator->current == NULL) {
		return NULL;
	}
	item = iterator->current->item;
	iterator->current = iterator->current->previous;

	return item;
}

void *list_current(list_iterator_t *iterator) {
	if(iterator->current == NULL) {
		return NULL;
	}
	return iterator->current->item;
}

void list_resetiterator(list_iterator_t *iterator) {
	iterator->current = iterator->list->head;
}

void list_setiterator_end(list_iterator_t *iterator) {
	iterator->current = iterator->list->tail;
}