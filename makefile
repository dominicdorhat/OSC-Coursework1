CC=gcc

CFLAGS = -std=c99 -lpthread 
SRCS = task4.c coursework.c linkedlist.c 
PROG = a

$(PROG) : 
	$(CC) $(SRCS) $(CFLAGS)
