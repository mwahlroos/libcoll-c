CC=gcc
CFLAGS= -ansi -Wall -Wextra -pedantic -g
SRC= hashtable.c hashtable_test.c linkedlist.c node.c
OBJS= hashtable.o hashtable_test.o linkedlist.o node.o
TEST_PROGRAM=hashtable_test

$TEST_PROGRAM:
	$(CC) $(CFLAGS) $(SRC) -o $(TEST_PROGRAM)

clean:
	rm -f $(OBJS) $(PROGRAM)

