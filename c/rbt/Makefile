SRC_MAIN	= bench_rbt.c # main_rbt.c
SRC_FILES	= $(SRC_MAIN) rbt.c ../common.c ../plot.c ../gettime.c ../list/linkedlist.c
HEADERS		= rbt.h ../common.h ../plot.h ../gettime.h ../list/list.h
CFLAGS		= -g -Wextra -Wall -lm

CMD_ARGS	= ./results/rbt_insert_bnch.txt ./results/rbt_search_bnch.txt ./results/rbt_sort_bnch.txt ./results/rbt_remove_bnch.txt ./results/rbt_getitem_bnch.txt ./results/rbt_iterator_bnch.txt
EXEC_LINE	= ./rbt.exe $(CMD_ARGS)


all: rbt


run:
	$(EXEC_LINE)

rbt: $(SRC_FILES) $(HEADERS) Makefile
	gcc $(SRC_FILES) $(CFLAGS) -o $@

valg:
	valgrind --leak-check=yes $(EXEC_LINE)

clean:
	rm -f *~ *.exe *.stackdump *.pdf *.dot rbt