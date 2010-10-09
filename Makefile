CC=gcc
CFLAGS= -ansi -Wall -pedantic -g
CFLAGS_LIB= -shared -fPIC
SRC= hashtable.c hashtable_test.c linkedlist.c node.c
OBJS= hashtable.o hashtable_test.o linkedlist.o node.o
LIB= datastruct.so
TEST_PROGRAM=hashtable_test

lib:
	$(CC) $(CFLAGS) $(CFLAGS_LIB) $(SRC) -o $(LIB)

$TEST_PROGRAM:
	$(CC) $(CFLAGS) $(SRC) -o $(TEST_PROGRAM)

clean:
	rm -f $(OBJS) $(LIB) $(TEST_PROGRAM)

