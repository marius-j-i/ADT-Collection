#include <stdio.h>
#include "../common.h"
#include "./avl.h"

#define MAXLENGTH 10


int cmpint(int *a , int *b) {
	return *a - *b;
}

char *strfunc(int *item) {
	return int_to_ascii(*item, 10);
}

int main(int argc, char **argv) {
	avl_t	*avl;
	int		num, *key, *item;

	if(argc < 2) {
		fatal_error("Usage: %s <value> \n", *argv);
	}

	avl = avl_create( (cmpfunc_t)cmpint );

	num = atoi(argv[1]);

	for(int i = 0; i < num; i++) {
		key = new_integer(i);
		item = new_integer(i);

		if(!avl_insert(avl, key, item)) {
			fatal_error("Error insert.");
		}
	}

	if(avl_size(avl) != num) {
		fatal_error("Structure size unproportional to inserts. ");
	}

	avl_print(avl, "AVL-Tree", (strfunc_t)strfunc);

	avl_destroy(avl, free, free);

	return 0;
}