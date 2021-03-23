/* Author: Marius Ingebrigtsen */

#ifndef __GRAPH_H_
#define __GRAPH_H_

#include "common.h"

/* Structure for graphs. */
typedef struct graph graph_t;
/* Structure for holding graph data about one plot. 
 * csv  - File-name to open containing comma-separated values. 
 * name - Name to display over a plot-figure. */
typedef struct graph_data {
	const char *csv, *name;
} graph_data_t;


/* Create .plot-file with given title. Will later produce .pdf-file with same title-name. */
graph_t *graph_create(const char *file_title);

/* Deallocate graph-structure and close plot-file, if open. */
void graph_destroy(graph_t *graph);

/* Set axis name. If new plot created this function must be called again to set labels for new graph-plot. */
void graph_axislabel(graph_t *graph, const char *x, const char *y);

/* Add new graph-plot to output-file on new page. 
 * Each different set of data source need one argument for plot-title, one for data-filename and another for name to attribute to data. 
 * If insufficient arguments are provided, this function will run into a segmentation-fault. 
 * 
 * char *title:		Title of graph-plot. 
 * int numplots:	Number of different data-sources in data-array. 
 * data:			Array of structs containing path to csv-file and name to attribute with file-data. */
void graph_newplot(graph_t *graph, const char *plot_title, graph_data_t *data, int num);

/* Write to and close .plot-file, then execute 'gnuplot' command to terminal. */
void graph_dograph(graph_t *graph);

#endif

