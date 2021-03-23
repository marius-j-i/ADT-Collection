### Author: Marius Ingebrigtsen ###

HASHFUNC	= ./lookup3.c
CHAIN_SRC	= ../rbt/rbt.c ../plot.c
SRC			= ../common.c ../gettime.c ./main_hash.c $(MAP_SRC) $(HASHFUNC) $(CHAIN_SRC)
CHAIN_HEADER= ../rbt/rbt.h ../plot.h
HEADERS		= ./map.h ./lookup3.h ../common.h ../gettime.h $(CHAIN_HEADER)
CFLAGS		= -g -Wall -Wextra -lm

MAP_SRC		= ./linear_probing/map.c
# MAP_SRC		= ./chained/map.c
# MAP_SRC		= ./chained/map.c

INDICATION	= 1

EXEC_LINE	= ./main_hash.exe map_linear_bench.txt $(INDICATION) LinearProbing
# EXEC_LINE	= ./main_hash.exe map_chain_link_bench.txt $(INDICATION) ChainLinked
# EXEC_LINE	= ./main_hash.exe map_chained_tree_bench.txt $(INDICATION) ChainTree


all: main_hash


run: 
	$(EXEC_LINE)

main_hash: $(SRC) $(HEADERS) Makefile
	gcc $(SRC) $(CFLAGS) -o $@

valg: main_hash
	valgrind --leak-check=yes $(EXEC_LINE)

plot:
	gnuplot plot_bench.plot

clean:
	rm -f *~ *.exe *.stackdump test main_hash
