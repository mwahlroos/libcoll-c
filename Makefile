VER_MAJOR=0
VER_MINOR=0
CC=gcc
LD=ld
SRC= src/*.c
INCLUDE_DIR= include
OBJS= *.o
TEST_SRC= test/*.c
TEST_PROG= libcoll_test
LIB_BASENAME= libcoll.so
LIB_SONAME= $(LIB_BASENAME).$(VER_MAJOR)
LIB_FILENAME= $(LIB_SONAME).$(VER_MINOR)
CFLAGS= -std=c99 -Wall -Wextra -pedantic -I$(INCLUDE_DIR)
CFLAGS_LIB= -shared -fPIC
LDFLAGS_LIB= -shared
VALGRIND=valgrind
VALGRIND_OPTS= --leak-check=full --trace-children=yes

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
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(TEST_PROG) -L. -lcoll -lcheck

debugtests: debug
	$(CC) $(CFLAGS) $(TEST_SRC) -g -DENABLE_DEBUG=1 -o $(TEST_PROG) -L. -lcoll -lcheck

runtests: tests
	@echo
	@echo Running unit tests...
	LD_LIBRARY_PATH=. ./$(TEST_PROG)

rundebugtests: debugtests
	@echo
	@echo Running unit tests...
	LD_LIBRARY_PATH=. ./$(TEST_PROG)

valgrind: debugtests
	# use CK_FORK=no to disable forking when running unit tests,
	# to get a single Valgrind report for the entire set of tests
	# (forking a separate process for each test suite would make
	# the output quite a bit less readable due to multiple reports)
	LD_LIBRARY_PATH=. CK_FORK=no $(VALGRIND) $(VALGRIND_OPTS) ./$(TEST_PROG)

clean:
	rm -f $(OBJS) $(LIB_SONAME) $(LIB_FILENAME) $(LIB_BASENAME) $(TEST_PROG)

