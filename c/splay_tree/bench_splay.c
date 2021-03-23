#include <math.h>

#include "../common.h"
#include "../gettime.h"
#include "./splay.h"

typedef struct data {
	int *key, *item;
} data_t;

#define START	1024		/* 2^10 */
#define MAXELEM	1048576+1	/* 2^20 + 1 */
#define REPEAT	10			/* For average calculation. */


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

static void assert_insert(char *bnch_file) {
	unsigned long long t1, t2, average, sum;
	FILE	*f;
	splay_t	*splay;
	data_t	*data;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Couldn't open file; %s. ", bnch_file);
	}
	fprintf(f, "Splay Insert\nElements, Time (microsec. average with %d number of repeats)\n\n", REPEAT);

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {

		sum = 0;

		for(int r = 0; r < REPEAT; r++) {

			splay = splay_create( (cmpfunc_t)cmpint );
			data = data_create(elem);

			t1 = gettime();
			for(int i = 0; i < elem; i++) {
				splay_insert(splay, data[i].key, data[i].item);
			}
			t2 = gettime();

			sum += t2 - t1;

			free(data);
			splay_destroy(splay, free, free);
		}
		fprintf(f, "%d, %d\n", elem, (int)(sum / REPEAT));

		average += sum / REPEAT;
	}
	average /= log2(MAXELEM) - log2(START);

	fprintf(f, "Overall average; \n%d\n", (int)average);

	fclose(f);
}

static void assert_search(char *bnch_file) {
	unsigned long long t1, t2, average, sum;
	FILE	*f;
	splay_t	*splay;
	data_t	*data;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Couldn't open file; %s. ", bnch_file);
	}
	fprintf(f, "Splay Search\nElements, Time (microsec. average with %d number of repeats)\n\n", REPEAT);

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {

		sum = 0;

		for(int r = 0; r < REPEAT; r++) {

			splay = splay_create( (cmpfunc_t)cmpint );
			data = insert_data(splay, elem);

			t1 = gettime();
			for(int i = 0; i < elem; i++) {
				if( splay_search(splay, data[i].key) == NULL) {
					fatal_error("Value not found.");
				}
			}
			t2 = gettime();

			sum += t2 - t1;

			free(data);
			splay_destroy(splay, free, free);
		}
		fprintf(f, "%d, %d\n", elem, (int)(sum / REPEAT));

		average += sum / REPEAT;
	}
	average /= log2(MAXELEM) - log2(START);

	fprintf(f, "Overall average; \n%d\n", (int)average);

	fclose(f);
}

int main(int argc, char **argv) {
	
	if(argc < 3) {
		printf("Usage: %s <result-file-insert> <result-file-search> \n", *argv);
	}

	assert_insert(argv[1]);
	assert_search(argv[2]);

	return 0;
}