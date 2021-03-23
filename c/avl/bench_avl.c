#include <math.h>
#include "../common.h"
#include "../gettime.h"
#include "./avl.h"

#define START 1024			/* 2^10 */
#define MAXELEM 1048576 + 1	/* 2^20 + 1 */
#define REPEAT 10

typedef struct data {
	int *key, *item;
} data_t;


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

static data_t *insert_data(avl_t *avl, int elem) {
	data_t *data;

	data = data_create(elem);

	for(int i = 0; i < elem; i++) {
		if(!avl_insert(avl, data[i].key, data[i].item)) {
			fatal_error("Duplicate insert.");
		}
	}
	return data;
}

static void assert_inserts(char *bnch_file) {
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
	num_elem_set = log2(MAXELEM) - log2(START);
	fprintf(f, "\n# Overall average of %d trials for each set of elements: \n%d \n", REPEAT, (int)(average / num_elem_set));

	fclose(f);
}

static void assert_searches(char *bnch_file) {
	unsigned long long t1, t2, sum, average, num_elem_set;
	FILE	*f;
	avl_t	*avl;
	data_t	*data;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Couldn't open file; %s. ", bnch_file);
	}
	fprintf(f, "# AVL Search Benchmarks \n# Elements, Time (microsec. on average of %d trials with corresponding nr. of elements) \n", REPEAT);

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {

		sum = 0;

		avl = avl_create( (cmpfunc_t)cmpint );
		data = insert_data(avl, elem);

		for(int r = 0; r < REPEAT; r++) {	

			t1 = gettime();
			for(int i = 0; i < elem; i++) {
				if(avl_search(avl, data[i].key) == NULL) {
					fatal_error("Value not found.");
				}
			}
			t2 = gettime();

			sum += t2 - t1;
			
		}
		free(data);
		avl_destroy(avl, free, free);
		
		fprintf(f, "%d, %d\n", elem, (int)(sum / REPEAT) );

		average += sum / REPEAT;
	}
	num_elem_set = log2(MAXELEM) - log2(START);
	fprintf(f, "\n# Overall average of %d trials for each set of elements: \n%d \n", REPEAT, (int)(average / num_elem_set));

	fclose(f);
}

static void assert_sort(char *bnch_file) {
	unsigned long long t1, t2, sum, average, num_elem_set;
	FILE	*f;
	avl_t	*avl;
	data_t	*data;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Couldn't open file; %s", bnch_file);
	}
	fprintf(f, "# AVL Sort Benchmarks \n# Elements, Time (microsec. on average of %d trials with corresponding nr. of elements) \n", REPEAT);

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {

		sum = 0;

		for(int r = 0; r < REPEAT; r++) {

			avl = avl_create( (cmpfunc_t)cmpint );
			data = insert_data(avl, elem);

			t1 = gettime();
			avl_sort(avl);
			t2 = gettime();

			sum += t2 - t1;

			free(data);
			avl_destroy(avl, free, free);
		}
		fprintf(f, "%d, %d\n", elem, (int)(sum / REPEAT) );

		average += sum / REPEAT;
	}
	num_elem_set = log2(MAXELEM) - log2(START);
	fprintf(f, "\n# Overall average of %d trials for each set of elements: \n%d \n", REPEAT, (int)(average / num_elem_set));
}

static void assert_iterator(char *bnch_file) {
	unsigned long long t1, t2, sum, average, num_elem_set;
	FILE	*f;
	avl_t	*avl;
	data_t	*data;
	avl_iterator_t *iterator;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Couldn't open file; %s", bnch_file);
	}
	fprintf(f, "# AVL Iterator Benchmarks \n# Elements, Time (microsec. on average of %d trials with corresponding nr. of elements) \n", REPEAT);

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {

		sum = 0;

		avl = avl_create( (cmpfunc_t)cmpint );
		data = insert_data(avl, elem);

		for(int r = 0; r < REPEAT; r++) {
			iterator = avl_createiterator(avl);

			t1 = gettime();
			while(avl_next(iterator) != NULL) {
			}
			t2 = gettime();

			sum += t2 - t1;

			avl_destroyiterator(iterator);
		}
		free(data);
		avl_destroy(avl, free, free);

		fprintf(f, "%d, %d\n", elem, (int)(sum / REPEAT) );

		average += sum / REPEAT;
	}
	num_elem_set = log2(MAXELEM) - log2(START);
	fprintf(f, "\n# Overall average of %d trials for each set of elements: \n%d \n", REPEAT, (int)(average / num_elem_set));
}

int main(int argc, char **argv) {

	if(argc < 5) {
		printf("Usage: %s <insert-bench-file> <search-bench-file> <sort-bench-file> <iterator-bench-file> ", *argv);
		return -1;
	}

	assert_inserts(argv[1]);
	assert_searches(argv[2]);
	assert_sort(argv[3]);
	assert_iterator(argv[4]);
	
	return 0;
}