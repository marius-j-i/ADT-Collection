# Author: Marius Ingebrigtsen


MAIN	= t.c
ADT_C	= avl/avl.c hashmaps/linear_probing/map.c list/linkedlist.c rbt/rbt.c splay_tree/splay.c

UTIL_C	= common.c gettime.c graph.c plot.c 
FILES	= $(UTIL_C) $(ADT_C) $(MAIN)

ADT_H	= avl/avl.h hashmaps/map.h list/list.h rbt/rbt.h splay_tree/splay.h

UTIL_H	= common.h gettime.h graph.h plot.h 
HEADERS	= $(UTIL_H) $(ADT_H)

OUT		= t
CFLAGS	= -g -Wall -Wextra - Wpedantic
#ARGS	= 


all: $(OUT)


$(OUT): $(FILES) $(HEADERS) Makefile
	@echo "Insert file with main in Makefile. "
	gcc $(FILES) -o $(OUT) $(CFLAGS) 


run: $(OUT)
	./$(OUT) $(ARGS)


clean:
	rm -rf $(OUT) *.exe *.stackdump 
