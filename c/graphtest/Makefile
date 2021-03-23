
SRC			= ../graph.c ./testgraph.c ../common.c 
HEADERS		= ../common.h ../graph.h
CFLAGS		= -g -Wall -Wextra -lm

EXEC_LINE	= ./graph 4 Exponential_Growth


all: graph

run:
	$(EXEC_LINE)

graph: $(SRC) $(HEADERS)
	gcc $(SRC) $(CFLAGS) -o $@

clean: 
	rm -rf *.exe *.exe.stackdump *.plot *.txt *.pdf graph