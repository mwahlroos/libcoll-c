VER_MAJOR=0
VER_MINOR=0
CC=gcc
LD=ld
CFLAGS= -std=c99 -Wall -Wextra -pedantic
CFLAGS_LIB= -shared -fPIC
LDFLAGS_LIB= -shared
SRC= hashmap.c linkedlist.c treemap.c node.c
OBJS= hashmap.o linkedlist.o treemap.o node.o
TEST_SRC= test/test.c
TEST_PROG= ccoll_test
LIB_BASENAME= libccoll.so
LIB_SONAME= $(LIB_BASENAME).$(VER_MAJOR)
LIB_FILENAME= $(LIB_SONAME).$(VER_MINOR)

so:
	$(CC) $(CFLAGS) $(CFLAGS_LIB) -O2 -c $(SRC)
	$(LD) $(LDFLAGS_LIB) -soname $(LIB_SONAME) -o $(LIB_FILENAME) -lc $(OBJS)
	ln -fs $(LIB_FILENAME) $(LIB_SONAME)
	ln -fs $(LIB_SONAME) $(LIB_BASENAME)

debug:
	$(CC) $(CFLAGS) $(CFLAGS_LIB) -DENABLE_DEBUG=1 -c -g $(SRC)
	$(LD) $(LDFLAGS_LIB) -soname $(LIB_SONAME) -o $(LIB_FILENAME) -lc $(OBJS)
	ln -fs $(LIB_FILENAME) $(LIB_SONAME)
	ln -fs $(LIB_SONAME) $(LIB_BASENAME)

tests: so
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(TEST_PROG) -L. -lccoll -lcheck

debugtests: debug
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(TEST_PROG) -L. -lccoll -lcheck

clean:
	rm -f $(OBJS) $(LIB_SONAME) $(LIB_FILENAME) $(LIB_BASENAME) $(TEST_PROG)

