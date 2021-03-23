#include "rbt.h"
#include "../list/list.h"
#include <string.h>

#define MAXLENGTH 10


typedef struct data {
	int *key, *item;
} data_t;


static int cmpint(int *a, int *b) {
	return *a - *b;
}

static list_t *tmp;

static char *strfunc(int *item) {
	char *strrep;

	strrep = int_to_ascii(*item, 10);

	list_addlast(tmp, strrep);

	return strrep;
}

static void apply_inserts(rbt_t *rbt, int num, data_t *data) {

	for(int i = 0; i < num; i++) {
		data[i].key		= new_integer(i);
		data[i].item	= new_integer(i); // generate_randomized_word(MAXLENGTH);

		if( !rbt_insert(rbt, data[i].key, data[i].item) ) {
			fatal_error("Key and item; \'%d\' & \'%d\', not inserted. \n", *data[i].key, *data[i].item);
		}
	}
	printf("\nAll \'%d\' insertions completed. \n", num);
}

static void apply_searches(rbt_t *rbt, int num, data_t *data) {

	for(int i = 0; i < num; i++) {

		if(rbt_search(rbt, data[i].key) == NULL) {
			fatal_error("Key; \'%d\', not in structure. \n", *data[i].key);
		}
	}
	printf("\nAll searches completed. \n");
}

static void apply_size(rbt_t *rbt, int num) {

	if(rbt_size(rbt, 1) != num) {
		fatal_error("\nDifference in size for insertions. \nReported size; \'%d\'. Expected size; \'%d\'. \n", rbt_size(rbt, 1), num);
	}
	printf("\nSize-up is proportional. \n");
}

static void apply_removals(rbt_t *rbt, int num, data_t *data) {
	int *item;

	for(int i = 1; i < num; i *= 2) {

		item = rbt_pop(rbt, data[i].key, free);
		
		if(item != NULL) {
			printf("Key; \'%d\', popped out item; \'%d\'. \n", *data[i].key, *item);
			free(item);
		} else {
			printf("Key; \'%d\', popped out item; \'%p\'. \n", *data[i].key, NULL);
		}
	}
	for(int i = 0; i < num; i++) {
		item = rbt_search(rbt, &i);
		if(item != NULL) {
			printf("%d -> ", *item);
		} else {
			printf("%p -> ", NULL);
		}
	}
	printf("\nAll removals completed.\n");
}

int main(int argc, char **argv) {
	rbt_t	*rbt;
	int		num;

	if(argc < 2) {
		printf("Usage: %s <value> \n", *argv);
		return -1;
	}

	rbt = rbt_create( (cmpfunc_t)cmpint );

	num = atoi(argv[1]);

	data_t data[num];


	apply_inserts(rbt, num, data);


	apply_searches(rbt, num, data);


	apply_size(rbt, num);


	tmp = list_create( (cmpfunc_t)cmpint );

	rbt_print(rbt, (strfunc_t)strfunc );
	printf("\nStructure printed. \n");


	getchar();


	apply_removals(rbt, num, data);

	rbt_print(rbt, (strfunc_t)strfunc );
	printf("\nStructure printed. \n");

	list_destroy(tmp, free);


	rbt_destroy(rbt, NULL, NULL);

	for(int i = 0; i < num; i++) {
		free(data[i].key);
		free(data[i].item);
	}


	return 0;
}