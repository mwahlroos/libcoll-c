CC=gcc
CFLAGS= -std=c99 -Wall -pedantic -g
CFLAGS_LIB_SO= -shared -fPIC
SRC= hashtable.c linkedlist.c node.c
OBJS= hashtable.o linkedlist.o node.o
LIB_SO= datastruct.so
HT_TEST= hashtable_test
LL_TEST= linkedlist_test


lib_so:
	$(CC) $(CFLAGS) $(CFLAGS_LIB_SO) $(SRC) -o $(LIB_SO)

ht_test:
	$(CC) $(CFLAGS) $(SRC) $(HT_TEST).c -o $(HT_TEST)

ll_test:
	$(CC) $(CFLAGS) $(SRC) $(LL_TEST).c -o $(LL_TEST)

clean:
	rm -f $(OBJS) $(LIB_SO) $(HT_TEST) $(LL_TEST)

