#include "../common.h"
#include "./splay.h"

typedef struct data {
	int *key, *item;
} data_t;


static int cmpint(int *a, int *b) {
	return *a - *b;
}

static char *strfunc(int *key) {
	return int_to_ascii(*key, 10);
}

static data_t *data_create(int num) {
	data_t *data;

	data = calloc(num, sizeof(data_t));
	if(data == NULL) {
		fatal_error("Out of memory.");
	}

	for(int i = 0; i < num; i++) {
		data[i].key = new_integer(i);
		data[i].item = new_integer(i);
	}
	
	return data;
}

static data_t *insert_data(splay_t *splay, int num) {
	data_t *data;

	data = data_create(num);

	for(int i = 0; i < num; i++) {
		if( !splay_insert(splay, data[i].key, data[i].item) ) {
			fatal_error("Duplicate insert.");
		}
	}

	return data;
}

static void assert_search(splay_t *splay, data_t *data, int num) {
	for(int i = 0; i < num; i++) {
		if( splay_search(splay, data[i].key) == NULL) {
			fatal_error("Value not found.");
		}
	}
}

static void assert_size(splay_t *splay, int num) {
	if(splay_size(splay) != num) {
		fatal_error("Size; \'%d\', unequal to Insert; \'%d\'. ", splay_size(splay), num);
	}
}

static void assert_print(splay_t *splay) {
	splay_print(splay, (strfunc_t)strfunc);
}

int main(int argc, char **argv) {
	splay_t	*splay;
	data_t	*data;
	int		num;

	if(argc < 2) {
		printf("Usage: %s <value> \n", *argv);
		return -1;
	}

	splay = splay_create( (cmpfunc_t)cmpint );
	num = atoi(argv[1]);
	data = insert_data(splay, num);

	assert_search(splay, data, num);
	assert_size(splay, num);
	assert_print(splay);

	free(data);
	splay_destroy(splay, free, free);

	return 0;
}