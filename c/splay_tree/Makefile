SRC_MAIN	= bench_splay.c # main_splay.c 
SRC_FILES	= $(SRC_MAIN) splay.c ../common.c ../plot.c ../gettime.c
HEADERS		= splay.h ../common.h ../plot.h ../gettime.h
CFLAGS		= -g -Wextra -Wall -lm

#EXEC_LINE	= ./splay.exe 32
EXEC_LINE	= ./splay.exe ./results/splay_insert.txt ./results/splay_search.txt


all: splay


run:
	$(EXEC_LINE)

splay: $(SRC_FILES) $(HEADERS) Makefile
	gcc $(SRC_FILES) $(CFLAGS) -o $@

clean:
	rm -f *~ *.o *.exe *.stackdump *.dot *.pdf splay