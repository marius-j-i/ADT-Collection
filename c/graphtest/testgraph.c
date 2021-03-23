/* Author: Marius Ingebrigtsen */

#include <math.h>
#include <string.h>
#include "../graph.h"

#define ROOF	16+1	/* 2^4 + 1 */
#define BUFFER	256


static char *path_create(int num) {
	char *path;

	path = calloc(BUFFER, sizeof(char));
	if(path == NULL) {
		fatal_error("Out of memory.");
	}
	sprintf(path, "txt%d.txt", num);

	return path;
}

static FILE *file_create(char *path) {
	FILE *f;

	f = fopen(path, "w");
	if(f == NULL) {
		fatal_error("Could not create file; %s", path);
	}
	return f;
}

static void write_exponent(FILE *f, int base) {
	fprintf(f, "# Exponential growth for Base; \'%d\' \n# Power, Base^Power (rounded down)\n", base);

	for(int exp = 0; exp < ROOF; exp++) {
		fprintf(f, "%d, %d\n", exp, (int)pow(base, exp) );
	}
	fflush(f);
}

static char **create_expfiles(int num) {
	FILE *files[num];
	char **paths;

	paths = calloc(num, sizeof(char*));
	if(paths == NULL) {
		fatal_error("Out of memory.");
	}

	for(int i = 0; i < num; i++) {
		paths[i] = path_create(i+1);

		files[i] = file_create(paths[i]);
		
		write_exponent(files[i], i+1);
		
		fclose(files[i]);
	}
	return paths;
}

static void create_graph(char **paths, int num, char *title) {
	graph_t *g;
	graph_data_t data[num];

	g = graph_create(title);

	graph_axislabel(g, "Power", "Base**Power");

	for(int i = 0; i < num; i++) {
		data[i].csf = paths[i];
		data[i].name = paths[i];
	}
	graph_newplot(g, "Exponent from 1 to 4", data, num);

	graph_newplot(g, "Same one all over...", data, num);	

	graph_dograph(g);

	graph_destroy(g);
}

int main(int argc, char **argv) {
	int	num;
	char **paths;

	if(argc < 3) {
		printf("Usage: %s <number-of-files-to-create> <graph-title> \n", *argv);
		return -1;
	}
	num = atoi(argv[1]);

	paths = create_expfiles(num);

	create_graph(paths, num, argv[2]);

	for(int i = 0; i < num; i++) {
		free(paths[i]);
	}
	free(paths);

	return 0;
}