LIST_SRC	= main_list.c linkedlist.c
HEADERS		= list.h

EXEC_LINE	= ./list.exe 32


all: list

run:
	$(EXEC_LINE)

list: $(LIST_SRC) $(HEADERS) Makefile
	gcc $(LIST_SRC) $(CFLAG) -o $@