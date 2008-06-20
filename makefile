CC=gcc
LD=gcc
CFLAGS=-g -Wall
LDFLAGS=-lm

OBJS= main.o acdll.o utils.o restaurant.o main_menu.o
PROG=main

all: $(OBJS)
	$(LD) -o $(PROG) $(OBJS) $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $<

test: $(PROG)
	@./$(PROG)

	
clean: 
	-rm -rf core *.o *.exe *~ "#"*"#" Makefile.bak $(PROG)
