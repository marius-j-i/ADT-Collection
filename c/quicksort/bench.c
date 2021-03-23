/* Author: Marius Ingebrigtsen */

#include "../gettime.h"
#include "../graph.h"
#include "./quicksort.h"

#include <math.h>

#define START	1024	/* 2^10 */
#define MAXELEM	1048577	/* 2^20 + 1 */
#define REPEAT	10


static int *array_create(int idx) {
	int *array;

	array = calloc(idx, sizeof(int));
	if(array == NULL) {
		fatal_error("Out of memory.");
	}
	return array;
}

static int *array_rand(int num) {
	int *array;

	array = array_create(num);

	for(int i = 0; i < num; i++) {
		array[i] = rand() % num;
	}
	return array;
}

static void bench_quicksort(const char *bnch_file) {
	unsigned long long t1, t2, sum, average;
	FILE *f;
	int *array;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Could not open file; \'%s\'", bnch_file);
	}
	fprintf(f, "# Elements, Time (in micro.sec.)\n");

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {
		printf("Sorting for %d elements... \n", elem);

		sum = 0;

		for(int r = 0; r < REPEAT; r++) {
			array = array_rand(elem);

			t1 = gettime();
			quicksort(array, elem);
			t2 = gettime();

			sum += t2 - t1;

			free(array);
		}
		average += sum / REPEAT;

		fprintf(f, "%d, %d\n", elem, (int)(sum / REPEAT) );
	}
	average /= log2(MAXELEM) - log2(START);
	
	fprintf(f, "# Overall average: \n%d\n", (int)average);

	fclose(f);
}

int cmpint(const void *a, const void *b) {
	int *x = (int*)a, *y = (int*)b;

	return *x - *y;
}

static void bench_qsort(const char *bnch_file) {
	unsigned long long t1, t2, sum, average;
	FILE *f;
	int *array;

	f = fopen(bnch_file, "w");
	if(f == NULL) {
		fatal_error("Could not open file; \'%s\'", bnch_file);
	}
	fprintf(f, "# Elements, Time (in micro.sec.)\n");

	average = 0;

	for(int elem = START; elem < MAXELEM; elem *= 2) {
		printf("Sorting for %d elements... \n", elem);

		sum = 0;

		for(int r = 0; r < REPEAT; r++) {
			array = array_rand(elem);

			t1 = gettime();
			qsort(array, elem, sizeof(int), cmpint);
			t2 = gettime();

			sum += t2 - t1;

			free(array);
		}
		average += sum / REPEAT;

		fprintf(f, "%d, %d\n", elem, (int)(sum / REPEAT) );
	}
	average /= log2(MAXELEM) - log2(START);
	
	fprintf(f, "# Overall average: \n%d\n", (int)average);

	fclose(f);
}

static void bench_graph(const char *quicksort_file, const char *qsort_file) {
	graph_t *g;
	graph_data_t gd[2];

	g = graph_create("Quicksort_vs_Qsort");

	graph_axislabel(g, "Elements", "Time");

	gd[0].csf	= quicksort_file;
	gd[0].name	= "Quicksort";
	gd[1].csf	= qsort_file;
	gd[1].name	= "Qsort";

	graph_newplot(g, "Quicksort vs. Qsort", gd, 2);

	graph_dograph(g);

	graph_destroy(g);
}

int main(int argc, const char **argv) {
	if(argc < 3) {
		printf("Usage: %s <quicksort-path> <qsort-path> \n", *argv);
		return -1;
	}
	bench_quicksort(argv[1]);

	bench_qsort(argv[2]);

	bench_graph(argv[1], argv[2]);

	return 0;
}