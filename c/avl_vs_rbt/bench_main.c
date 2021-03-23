/* Author: Marius Ingebrigtsen */

#include <math.h>

#include "../common.h"
#include "../gettime.h"
#include "../graph.h"
#include "../avl/avl.h"
#include "../rbt/rbt.h"

#define START 1024			/* 2^10 */
#define MAXELEM 1048576 + 1	/* 2^20 + 1 */
#define REPEAT 10

typedef struct data {
	int *key, *item;
} data_t;

typedef int (*insertfunc_t)(void *interface, void *key, void *item);


static int cmpint(int *a, int *b) {
	return *a - *b;
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

static data_t *insert_avl_data(avl_t *avl, int elem) {
	data_t *data;

	data = data_create(elem);

	for(int i = 0; i < elem; i++) {
		if(!avl_insert(avl, data[i].key, data[i].item)) {
			fatal_error("Duplicate insert.");
		}
	}
	return data;
}

static data_t *insert_rbt_data(rbt_t *rbt, int elem) {
	data_t *data;

	data = data_create(elem);

	for(int i = 0; i < elem; i++) {
		if(!rbt_insert(rbt, data[i].key, data[i].item)) {
			fatal_error("Duplicate insert.");
		}
	}
	return data;
}

static void assert_avl_inserts(char *bnch_file) {
	unsigned long long t1, t2, sum, average, num_elem_set;
	FILE	*f;
	avl_t	*avl;
	data_t	*data;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Couldn't open file; %s. ", bnch_file);
	}
	fprintf(f, "# AVL Insert Benchmarks \n# Elements, Time (microsec. on average of %d trials with corresponding nr. of elements) \n", REPEAT);

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {
		
		sum = 0;

		for(int r = 0; r < REPEAT; r++) {

			printf("AVL-Insert: elements \'%d\' - repeat \'%d\'\n", elem, r+1);

			avl = avl_create( (cmpfunc_t)cmpint );
			data = data_create(elem);

			t1 = gettime();
			for(int i = 0; i < elem; i++) {
				avl_insert(avl, data[i].key, data[i].item);
			}
			t2 = gettime();

			sum += t2 - t1;

			free(data);
			avl_destroy(avl, free, free);
		}
		fprintf(f, "%d, %d\n", elem, (int)(sum / REPEAT) );
		
		average += sum / REPEAT;
	}
	num_elem_set = ceil(log2(MAXELEM) - log2(START));
	fprintf(f, "\n# Overall average of %d trials for each set of elements: \n# %d \n", REPEAT, (int)(average / num_elem_set));

	fclose(f);
}

static void assert_rbt_inserts(char *bnch_file) {
	unsigned long long t1, t2, sum, average, num_elem_set;
	FILE	*f;
	rbt_t	*rbt;
	data_t	*data;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Could not open file; \"%s\"",  bnch_file);
	}
	fprintf(f, "# RBT Insert Benchmarks \n# Elements, Time (microsec. on average of %d trials with corresponding nr. of elements) \n", REPEAT);

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {
		
		sum = 0;

		for(int r = 0; r < REPEAT; r++) {

			printf("RBT-Insert: elements \'%d\' - repeat \'%d\'\n", elem, r+1);

			rbt = rbt_create( (cmpfunc_t)cmpint );
			data = data_create(elem);

			t1 = gettime();
			for(int i = 0; i < elem; i++) {
				rbt_insert(rbt, data[i].key, data[i].item);
			}
			t2 = gettime();

			sum += t2 - t1;

			free(data);
			rbt_destroy(rbt, free, free);
		}
		fprintf(f, "%d, %d\n", elem, (int)(sum / REPEAT) );
		
		average += sum / REPEAT;
	}
	num_elem_set = ceil(log2(MAXELEM) - log2(START));
	fprintf(f, "\n# Overall average of %d trials for each set of elements: \n# %d \n", REPEAT, (int)(average / num_elem_set));

	fclose(f);
}

static void assert_avl_search(char *bnch_file) {
	unsigned long long t1, t2, sum, average, num_elem_set;
	FILE	*f;
	avl_t	*avl;
	data_t	*data;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Could not create file; %s.", bnch_file);
	}
	fprintf(f, "# AVL Search Benchmarks \n# Elements, Time (microsec. on average of %d trials with corresponding nr. of elements) \n", REPEAT);

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {

		sum = 0;

		for(int r = 0; r < REPEAT; r++) {

			printf("AVL-Search: elements \'%d\' - repeat \'%d\'\n", elem, r+1);

			avl = avl_create( (cmpfunc_t)cmpint );
			data = insert_avl_data(avl, elem);

			t1 = gettime();
			for(int i = 0; i < elem; i++) {
				if(avl_search(avl, data[i].key) == NULL) {
					fatal_error("Value not found.");
				}
			}
			t2 = gettime();

			sum += t2 - t1;

			free(data);
			avl_destroy(avl, free, free);
		}
		fprintf(f, "%d, %d\n", elem, (int)(sum / REPEAT));

		average += sum / REPEAT;
	}
	num_elem_set = ceil(log2(MAXELEM) - log2(START));

	fprintf(f, "\n# Overall average of %d trials for each set of elements: \n# %d \n", REPEAT, (int)(average / num_elem_set));
}

static void assert_rbt_search(char *bnch_file) {
	unsigned long long t1, t2, sum, average, num_elem_set;
	FILE	*f;
	rbt_t	*rbt;
	data_t	*data;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Could not create file; %s.", bnch_file);
	}
	fprintf(f, "# RBT Search Benchmarks \n# Elements, Time (microsec. on average of %d trials with corresponding nr. of elements) \n", REPEAT);

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {

		sum = 0;

		for(int r = 0; r < REPEAT; r++) {

			printf("RBT-Search: elements \'%d\' - repeat \'%d\'\n", elem, r+1);

			rbt = rbt_create( (cmpfunc_t)cmpint );
			data = insert_rbt_data(rbt, elem);

			t1 = gettime();
			for(int i = 0; i < elem; i++) {
				if(rbt_search(rbt, data[i].key) == NULL) {
					fatal_error("Value not found.");
				}
			}
			t2 = gettime();

			sum += t2 - t1;

			free(data);
			rbt_destroy(rbt, free, free);
		}
		fprintf(f, "%d, %d\n", elem, (int)(sum / REPEAT));

		average += sum / REPEAT;
	}
	num_elem_set = ceil(log2(MAXELEM) - log2(START));

	fprintf(f, "\n# Overall average of %d trials for each set of elements: \n# %d \n", REPEAT, (int)(average / num_elem_set));
}

static void create_graph(char *insert_avl, char *insert_rbt, char *search_avl, char *search_rbt) {
	graph_t *g;
	graph_data_t data[2];

	g = graph_create("AVL_vs_RBT_Insert");

	graph_axislabel(g, "Elements", "Microsec.");

	data[0].csf		= insert_avl;
	data[0].name	= "AVL-Insert";

	data[1].csf		= insert_rbt;
	data[1].name	= "RBT-Insert";

	graph_newplot(g, "AVL- vs. RBT-Insert", data, 2);

	data[0].csf		= search_avl;
	data[0].name	= "AVL-Insert";

	data[1].csf		= search_rbt;
	data[1].name	= "RBT-Insert";

	graph_newplot(g, "AVL- vs. RBT-Search", data, 2);

	graph_dograph(g);

	graph_destroy(g);
}

int main(int argc, char **argv) {
	
	if(argc < 5) {
		printf("Usage: %s <avl-insert-file> <rbt-insert-file> <avl-search-file> <rbt-search-file> \n", *argv);
		return -1;
	}

	printf("\nAsserting AVL-Insert...\n");
	assert_avl_inserts(argv[1]);

	printf("\nAsserting RBT-Insert...\n");
	assert_rbt_inserts(argv[2]);

	printf("\nAsserting AVL-Search...\n");
	assert_avl_search(argv[3]);

	printf("\nAsserting RBT-Search...\n");
	assert_rbt_search(argv[4]);

	printf("\nConstructing graph...\n");
	create_graph(argv[1], argv[2], argv[3], argv[4]);

	printf("Done.\n");

	return 0;
}