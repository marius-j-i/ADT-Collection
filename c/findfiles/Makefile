# Author: Marius Ingebrigtsen

FIND	= find
SRC		= ../common.c ../list/linkedlist.c index.c lookup3.c map.c query.c set.c find.c
HEADERS	= ../common.h ../list/list.h index.h lookup3.h map.h query.h set.h
CFLAGS	= -Wall -Wextra -g -lm

ARGS	= . set rbt


all: $(FIND)


$(FIND): $(SRC) $(HEADERS) Makefile
	gcc $(SRC) -o $@ $(CFLAGS)

run:
	./$(FIND) $(ARGS)

valg: $(FIND)
	valgrind --leak-check=yes --show-leak-kinds=all ./$< $(ARGS)

clean:
	rm -rf *.exe *.stackdump $(FIND)
