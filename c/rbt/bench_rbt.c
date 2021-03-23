/* Author: Marius Ingebrigtsen */
#include <math.h>

#include "../common.h"
#include "../gettime.h"
#include "./rbt.h"

#define START	1024		/* 2^10 */
#define MAXELEM	1048576+1	/* 2^20 + 1 */
#define REPEAT	10

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

static data_t *insert_data(rbt_t *rbt, int num) {
	data_t *data;

	data = data_create(num);

	for(int i = 0; i < num; i++) {
		if( !rbt_insert(rbt, data[i].key, data[i].item) ) {
			fatal_error("Duplicate insert.");
		}
	}

	return data;
}

static void assert_insert(char *bnch_file) {
	unsigned long long t1, t2, sum, average, num_elem_set;
	FILE	*f;
	rbt_t	*rbt;
	data_t	*data;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Out of memory.");
	}
	fprintf(f, "# RBT Insert Benchmarks \n# Elements, Time (microsec. average for %d trials per set of elements) \n", REPEAT);

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {

		sum = 0;

		for(int r = 0; r < REPEAT; r++) {

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
	num_elem_set = log2(MAXELEM) - log2(START);
	fprintf(f, "\n# Overall average for each set of elements with %d trials: \n%d", REPEAT, (int)(average / num_elem_set) );

	fclose(f);
}

static void assert_search(char *bnch_file) {
	unsigned long long t1, t2, sum, average, num_elem_set;
	FILE	*f;
	rbt_t	*rbt;
	data_t	*data;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Out of memory.");
	}
	fprintf(f, "# RBT Search Benchmarks \n# Elements, Time (microsec. average for %d trials per set of elements) \n", REPEAT);

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {

		sum = 0;

		rbt = rbt_create( (cmpfunc_t)cmpint );
		data = insert_data(rbt, elem);

		for(int r = 0; r < REPEAT; r++) {

			t1 = gettime();
			for(int i = 0; i < elem; i++) {
				if(rbt_search(rbt, data[i].key) == NULL) {
					fatal_error("Value not found.");
				}
			}
			t2 = gettime();

			sum += t2 - t1;

		}
		free(data);
		rbt_destroy(rbt, free, free);

		fprintf(f, "%d, %d\n", elem, (int)(sum / REPEAT) );

		average += sum / REPEAT;
	}
	num_elem_set = log2(MAXELEM) - log2(START);
	fprintf(f, "\n# Overall average for each set of elements with %d trials: \n%d", REPEAT, (int)(average / num_elem_set) );

	fclose(f);
}

static void assert_sort(char *bnch_file) {
	unsigned long long t1, t2, sum, average, num_elem_set;
	FILE	*f;
	rbt_t	*rbt;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Out of memory.");
	}
	fprintf(f, "# RBT Sort Benchmarks \n# Elements, Time (microsec. average for %d trials per set of elements) \n", REPEAT);

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {

		sum = 0;

		for(int r = 0; r < REPEAT; r++) {

			rbt = rbt_create( (cmpfunc_t)cmpint );
			free(insert_data(rbt, elem));

			t1 = gettime();
			rbt_sort(rbt);
			t2 = gettime();

			sum += t2 - t1;

			rbt_destroy(rbt, free, free);
		}
		fprintf(f, "%d, %d\n", elem, (int)(sum / REPEAT) );

		average += sum / REPEAT;
	}
	num_elem_set = log2(MAXELEM) - log2(START);
	fprintf(f, "\n# Overall average for each set of elements with %d trials: \n%d", REPEAT, (int)(average / num_elem_set) );

	fclose(f);
}

static void assert_remove(char *bnch_file) {
	unsigned long long t1, t2, sum, average, num_elem_set;
	FILE	*f;
	rbt_t	*rbt;
	data_t	*data;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Out of memory.");
	}
	fprintf(f, "# RBT Remove Benchmarks \n# Elements, Time (microsec. average for %d trials per set of elements) \n", REPEAT);

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {

		sum = 0;

		for(int r = 0; r < REPEAT; r++) {

			rbt = rbt_create( (cmpfunc_t)cmpint );
			data = insert_data(rbt, elem);

			t1 = gettime();
			for(int i = 0; i < elem; i++) {
				if( !rbt_remove(rbt, data[i].key, free, free) ) {
					fatal_error("Key not in structure.");
				}
			}
			t2 = gettime();

			sum += t2 - t1;

			free(data);
			rbt_destroy(rbt, free, free);
		}
		fprintf(f, "%d, %d\n", elem, (int)(sum / REPEAT) );

		average += sum / REPEAT;
	}
	num_elem_set = log2(MAXELEM) - log2(START);
	fprintf(f, "\n# Overall average for each set of elements with %d trials: \n%d", REPEAT, (int)(average / num_elem_set) );

	fclose(f);
}

static void assert_getitem(char *bnch_file) {
	unsigned long long t1, t2, sum, average, num_elem_set;
	FILE *f;
	rbt_t *rbt;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Out of memory.");
	}
	fprintf(f, "# RBT Get-Item Benchmarks \n# Elements, Time (microsec. average for %d trials per set of elements) \n", REPEAT);

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {

		printf("\'%d\' elements... \n", elem);

		sum = 0;

		rbt = rbt_create( (cmpfunc_t)cmpint );
		free(insert_data(rbt, elem));
		rbt_sort(rbt);

		for(int r = 0; r < REPEAT; r++) {


			t1 = gettime();
			for(int i = 1; i < elem; i *= 2) {
				if(rbt_getitem(rbt, i) == NULL) {
					fatal_error("\'%d\' out of bounds.", i);
				}
			}
			t2 = gettime();

			sum += t2 - t1;
		}
		rbt_destroy(rbt, free, free);

		fprintf(f, "%d, %d\n", elem, (int)(sum / REPEAT) );

		average += sum / REPEAT;
	}
	num_elem_set = log2(MAXELEM) - log2(START);
	fprintf(f, "\n# Overall average for each set of elements with %d trials: \n%d", REPEAT, (int)(average / num_elem_set) );

	fclose(f);
}

static void assert_iterator(char *bnch_file) {
	unsigned long long t1, t2, sum, average, num_elem_set;
	FILE	*f;
	rbt_t	*rbt;
	rbt_iterator_t *iterator;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Out of memory.");
	}
	fprintf(f, "# RBT Iterator Benchmarks \n# Elements, Time (microsec. average for %d trials per set of elements) \n", REPEAT);

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {

		sum = 0;

		rbt = rbt_create( (cmpfunc_t)cmpint );
		free(insert_data(rbt, elem));

		for(int r = 0; r < REPEAT; r++) {
			iterator = rbt_createiterator(rbt);

			t1 = gettime();
			while(rbt_next(iterator) != NULL) {
			}
			t2 = gettime();

			sum += t2 - t1;

			rbt_destroyiterator(iterator);
		}
		rbt_destroy(rbt, free, free);

		fprintf(f, "%d, %d\n", elem, (int)(sum / REPEAT) );

		average += sum / REPEAT;
	}
	num_elem_set = log2(MAXELEM) - log2(START);
	fprintf(f, "\n# Overall average for each set of elements with %d trials: \n%d", REPEAT, (int)(average / num_elem_set) );

	fclose(f);
}

int main(int argc, char **argv) {
	
	if(argc < 7) {
		printf("Usage: %s <insert-file> <search-file> <sort-file> <remove-file> <getitem-file> <iterator-file> \n", *argv);
		return -1;
	}
	
	assert_insert(argv[1]);
	assert_search(argv[2]);
	assert_sort(argv[3]);
	assert_remove(argv[4]);
	assert_getitem(argv[5]);
	assert_iterator(argv[6]);
	
	return 0;
}