CC=gcc
CFLAGS= -ansi -Wall -pedantic -g
CFLAGS_LIB= -shared -fPIC
SRC= hashtable.c linkedlist.c node.c
OBJS= hashtable.o linkedlist.o node.o
LIB= datastruct.so
HT_TEST=hashtable_test
LL_TEST=linkedlist_test

lib:
	$(CC) $(CFLAGS) $(CFLAGS_LIB) $(SRC) -o $(LIB)

ht_test: lib
	$(CC) $(CFLAGS) $(HT_TEST).c -o $(HT_TEST)

ll_test: lib
	$(CC) $(CFLAGS) $(LL_TEST).c -o $(LL_TEST)

clean:
	rm -f $(OBJS) $(LIB) $(HT_TEST) $(LL_TEST)

