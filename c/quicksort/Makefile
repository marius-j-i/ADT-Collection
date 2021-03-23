SRC_FILES	= ../common.c ../gettime.c ../graph.c ./quicksort.c ./bench.c
HEADERS		= ../common.h ../gettime.h ../graph.h ./quicksort.h
CFLAGS		= -g -Wall -Wextra -lm

EXEC_LINE	= ./quicksort.exe quicksort.txt qsort.txt

all: quicksort

run: 
	$(EXEC_LINE)

quicksort: $(HEADERS) $(SRC_FILES)
	gcc $(SRC_FILES) $(CFLAGS) -o $@

clean:
	rm -rf *.exe *.exe.stackdump *.pdf quicksort