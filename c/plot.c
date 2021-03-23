/* Author: Aage Kvalnes <aage@cs.uit.no> */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "plot.h"

struct plot {
    FILE *f;
    char filename[128];
};


plot_t *plot_create(char *name)
{
    plot_t *plot;
    char cmd[256];

    plot = malloc(sizeof(plot_t));
    if (plot == NULL)
        goto error;
    
    sprintf(plot->filename, "%s.dot", name);
    plot->f = fopen(plot->filename, "w");
    if (plot->f == NULL)
        goto error;

    fprintf(plot->f, "digraph structs {\n");
    fprintf(plot->f, "rankdir=TB;\n");
    fprintf(plot->f, "node [shape=circle];\n");
    fprintf(plot->f, "size=\"7.5,10\";\n");
    fflush(plot->f);
    
    return plot;
    
 error:
    if (plot != NULL) {
        if (plot->f != NULL)
            fclose(plot->f);
        sprintf(cmd, "rm -f %s", plot->filename);
        system(cmd);
        free(plot);
    }

    return NULL;
}


void plot_destroy(plot_t *plot) {
    free(plot);
}


void plot_addlink(plot_t *plot, char *from, char *to)
{
    fprintf(plot->f, "%s->%s;\n", from, to);
    fflush(plot->f);
}


void plot_addlink2(plot_t *plot, void *from, void *to, char *from_label, char *to_label, char *color_one, char *color_two)
{
	fprintf(plot->f, "%ld [fillcolor=\"%s\"] [label=\"%s\"];\n", (long)from, color_one, from_label);
    fprintf(plot->f, "%ld [fillcolor=\"%s\"] [label=\"%s\"];\n", (long)to, color_two, to_label);
    fprintf(plot->f, "%ld->%ld;\n", (long)from, (long)to);
    fflush(plot->f);
}


void plot_doplot(plot_t *plot)
{
    char cmd[512];

    fprintf(plot->f,"}\n");
    fflush(plot->f);

    sprintf(cmd, "dot -Tpdf %s -o %s.pdf", plot->filename, plot->filename);
    system(cmd);
    //sprintf(cmd, "xpdf %s.pdf", plot->filename);
    //system(cmd);
}
