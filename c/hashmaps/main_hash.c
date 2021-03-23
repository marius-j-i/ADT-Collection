/* Author: Marius Ingebrigtsen */
#include "../common.h"
#include "../gettime.h"
#include "map.h"
#include "lookup3.h"

#define MAXWORDLENGTH 10
#define START 128
#define MAXENTRIES 1048576+1 /* 2^20 */


typedef struct data {
	int *key;
	char *value;
} data_t;


static int cmpint(int *a, int *b) {
	return *a - *b;
}

static data_t *data_create(int num) {
	data_t *data;

	data = (data_t*)malloc(sizeof(data_t) * num);
	if(data == NULL) {
		fatal_error("Out of memory.\n");
	}

	for(int i = 0; i < num; i++) {
		data[i].key = new_integer(i);
		data[i].value = generate_randomized_word(MAXWORDLENGTH);
	}

	return data;
}

static void bench_map_put(FILE *f, char *impl) {
	data_t 	*data;
	map_t	*map;
	unsigned long long t1, t2, time, average;

	fprintf(f, "# Time for putting number of elements into hashmap for %s-implementation \n# Elements, Time \n", impl);

	for(int elements = START; elements < MAXENTRIES; elements *= 2) {

		data = data_create(elements);

		map = map_create( (cmpfunc_t)cmpint, (hashfunc_t)lookup3 );

		time = 0;

		printf("Benching for \'%d\'-elements. \n", elements);
		t1 = gettime();
		for(int elem = 0; elem < elements; elem++) {

			map_put(map, data[elem].key, data[elem].value);

		}
		t2 = gettime();

		time = t2 - t1;
		printf("Benching for \'%d\'-elements. \n", elements);
		printf("Time for benching \'%d\'-elements; \'%llu\'. \n", elements, time);

		fprintf(f, "%d, %d\n", elements, (int)time);

		average += time;

		map_destroy(map, free, free);

		free(data);
	}
	average /= MAXENTRIES;

	printf("Average Time For all elements: %d\n", (int)average);
}

static void bench_map_get(FILE *f, char *impl) {
	unsigned long long t1, t2, time, average;
	map_t *map;
	data_t *data;

	fprintf(f, "# Time for putting number of elements into hashmap for %s-implementation \n# Elements, Time \n", impl);

	for(int elements = START; elements < MAXENTRIES; elements *= 2) {

		map = map_create( (cmpfunc_t)cmpint, (hashfunc_t)lookup3 );

		data = data_create(elements);

		for(int i = 0; i < elements; i++) {
			map_put(map, data[i].key, data[i].value);
		}

		printf("Benching for \'%d\'-elements. \n", elements);
		t1 = gettime();
		for(int elem = 0; elem < elements; elem++) {

			if(map_get(map, data[elem].key) == NULL) {
				fatal_error("Map error; key miss. \n");
			}

		}
		t2 = gettime();

		time = t2 - t1;
		printf("Time for benching \'%d\'-elements; \'%llu\'. \n", elements, time);

		fprintf(f, "%d, %d\n", elements, (int)time);

		average += time;

		free(data);

		map_destroy(map, free, free);
	}
	average /= MAXENTRIES;

	printf("Average Time For all elements: %d\n", (int)average);
}

int main(int argc, char **argv) {
	FILE	*f;
	char	*result_path, *implementation;
	int		benchmark_indicator;

	if(argc < 4) {
		printf("Usage: %s <res-file> <indication> <implementation> \n", *argv);
		return -1;
	}
	result_path			= argv[1];
	benchmark_indicator	= atoi( argv[2] );
	implementation		= argv[3];

	f = fopen(result_path, "w");
	if(f == NULL) {
		fatal_error("Couldn't open file; \'%s\'. \n", result_path);
	}

	if(benchmark_indicator == 1) {
		bench_map_put(f, implementation);
	}
	else if(benchmark_indicator == 2) {
		bench_map_get(f, implementation);
	}

	fclose(f);

	return 0;
}