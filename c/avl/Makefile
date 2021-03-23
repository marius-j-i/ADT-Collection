HEADERS		= ./avl.h ../common.h ../plot.h ../gettime.h
MAIN_SRC	= ./bench_avl.c # ./main_avl.c
SRC_FILES	= $(MAIN_SRC) ./avl.c ../common.c ../plot.c ../gettime.c
CFLAGS		= -g -Wall -Wextra -lm

GRAPH_PLOT	= graph.plot

EXEC_LINE	= ./avl.exe ./results/avl_insert_bnch.txt ./results/avl_search_bnch.txt ./results/avl_sort_bnch.txt ./results/avl_iterator_bnch.txt


all: avl

run: 
	$(EXEC_LINE)

avl: $(SRC_FILES) $(HEADERS)
	gcc $(SRC_FILES) $(CFLAGS) -o $@

plot:
	gnuplot $(GRAPH_PLOT)

clean:
	rm -f *~ *.exe *.stackdump *.pdf *.dot avl