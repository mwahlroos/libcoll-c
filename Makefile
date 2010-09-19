CC=gcc
CFLAGS= -ansi -Wall -pedantic
SRC= hashtable.c hashtable_test.c linkedlist.c
OBJS= hashtable.o hashtable_test.o linkedlist.o
PROGRAM=hashtable_test

$PROGRAM:
	$(CC) $(CFLAGS) $(SRC) -o $(PROGRAM)

clean:
	rm -f $(OBJS) $(PROGRAM)

