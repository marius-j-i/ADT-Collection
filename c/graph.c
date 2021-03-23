/* Author: Marius Ingebrigtsen */

#include <stdarg.h>
#include <string.h>
#include "graph.h"

#define INITIAL 8	/* Initial number of graph-plots. */


/* Struct for each graph-plot. */
typedef struct gplot {
	char	*title, **csv, **datanames;
	int		numdata;
} gplot_t;

/* Graph Structure: */
struct graph {
	char	*file, *output, *xlab, *ylab;
	FILE	*f;
	gplot_t	**gplots;
	int		numplots, maxplots;
};


/* Graph Create: */
graph_t *graph_create(const char *title) {
	graph_t	*graph;

	graph = calloc(1, sizeof(graph_t));
	if(graph == NULL) {
		fatal_error("Out of memory.");
	}
	graph->file		= concatenate_strings(2, title, ".plot");
	graph->output	= concatenate_strings(2, title, ".pdf");
	graph->xlab		= strdup("x");
	graph->ylab		= strdup("y");

	graph->gplots = calloc(INITIAL, sizeof(gplot_t*));
	if(graph->gplots == NULL) {
		fatal_error("Out of memory.");
	}
	graph->maxplots = INITIAL;

	return graph;
}

/* Graph Destroy: */
void graph_destroy(graph_t *graph) {

	free(graph->file);
	free(graph->output);
	free(graph->xlab);
	free(graph->ylab);

	for(int i = 0; i < graph->numplots; i++) {
		for(int k = 0; k < graph->gplots[i]->numdata; k++) {
			free(graph->gplots[i]->csv[k]);
			free(graph->gplots[i]->datanames[k]);
		}
		free(graph->gplots[i]->title);
		free(graph->gplots[i]->csv);
		free(graph->gplots[i]->datanames);
		free(graph->gplots[i]);
	}
	free(graph->gplots);
	free(graph);
}

/* Graph Axis-Label: */
void graph_axislabel(graph_t *graph, const char *x, const char *y) {
	free(graph->xlab);
	free(graph->ylab);
	graph->xlab = strdup(x);
	graph->ylab = strdup(y);
}

/* Graph New Graph: */
static gplot_t *gplot_create(const char *title, graph_data_t *data, int num) {
	gplot_t *gplot;

	gplot = calloc(1, sizeof(gplot_t));
	if(gplot == NULL) {
		fatal_error("Out of memory.");
	}

	gplot->csv = calloc(num, sizeof(char*));
	if(gplot->csv == NULL) {
		fatal_error("Out of memory.");
	}

	gplot->datanames = calloc(num, sizeof(char*));
	if(gplot->datanames == NULL) {
		fatal_error("Out of memory.");
	}

	for(int i = 0; i < num; i++) {
		// printf("csv: \'%s\' \nname: \'%s\' \n", data[i].csv, data[i].name);fflush(stdout);
		gplot->csv[i]		= strdup(data[i].csv);
		gplot->datanames[i]	= strdup(data[i].name);
	}
	gplot->title	= strdup(title);
	gplot->numdata	= num;

	return gplot;
}

void graph_newplot(graph_t *graph, const char *title, graph_data_t *data, int num) {
	gplot_t	*gplot;

	if(graph->numplots >= graph->maxplots) { /* If to-be-inserted gplots exceed current capacity for gplots. */
		graph->maxplots	*= 2;
		graph->gplots	= realloc(graph->gplots, sizeof(gplot_t*) * graph->maxplots);
	}
	gplot = gplot_create(title, data, num);

	graph->gplots[graph->numplots] = gplot;

	graph->numplots++;
}

/* Graph Do-Graph: */
static void gplot_write(FILE *f, gplot_t *gplot) {
	fprintf(f, "set title \"%s\"\n", gplot->title);

	fprintf(f, "%s", "plot");

	for(int i = 0; i < gplot->numdata; i++) {
		fprintf(f, "\t\"%s\" w linespoints lw 2 t \"%s\"", gplot->csv[i], gplot->datanames[i]);
		
		if(i+1 < gplot->numdata) {
			fprintf(f, ", \\\n\t");
		}
	}
	fprintf(f, "%s", "\n\n");
	fflush(f);
}

void graph_dograph(graph_t *graph) {
	char cmd[256];

	graph->f = fopen(graph->file, "w");
	if(graph->f == NULL) {
		fatal_error("Could not create graph.");
	}

	fprintf(graph->f, "set terminal pdf \nset output \"%s\" \nset autoscale \nset xlabel \"%s\"\nset ylabel \"%s\"\nset key left\n\n", graph->output, graph->xlab, graph->ylab);
	fflush(graph->f);

	for(int i = 0; i < graph->numplots; i++) {
		gplot_write(graph->f, graph->gplots[i]);
	}
	err = fclose(graph->f);
	if(err) {
		fatal_error("Could not close graph-file; \'%s\'. ", graph->file);
	}

	sprintf(cmd, "gnuplot %s", graph->file);
	system(cmd);
}
