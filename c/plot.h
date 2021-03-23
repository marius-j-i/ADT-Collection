/* Author: Aage Kvalnes <aage@cs.uit.no> */
#ifndef PLOT_H
#define PLOT_H

#include "./common.h"


typedef struct plot plot_t;

plot_t *plot_create(char *name);

void plot_destroy(plot_t *plot);

void plot_addlink(plot_t *plot, char *from, char *to);

void plot_addlink2(plot_t *plot, void *from, void *to, char *from_label, char *to_label, char *color_one, char *color_two);

void plot_doplot(plot_t *plot);


#endif  /* PLOT_H */
