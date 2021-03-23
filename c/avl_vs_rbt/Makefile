# Author: Marius Ingebrigtsen

SRC			= ./bench_main.c ../common.c ../gettime.c ../graph.c ../avl/avl.c ../rbt/rbt.c ../plot.c
HEADERS		= ../common.h ../gettime.h ../graph.h ../avl/avl.h ../rbt/rbt.h ../plot.h
CFLAGS		= -g -Wall -Wextra -lm

EXEC_LINE	= ./bench results/avl_insert.txt results/rbt_insert.txt results/avl_search.txt results/rbt_search.txt


all: bench

run:
	$(EXEC_LINE)

bench: $(SRC) $(HEADERS)
	gcc $(SRC) $(CFLAGS) -o $@

clean:
	rm -rf *.exe *.exe.stackdump *.plot *.pdf bench