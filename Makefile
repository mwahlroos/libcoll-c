VER_MAJOR=0
VER_MINOR=0

CC=gcc
LD=ld

SRC= src/*.c
INCLUDE_DIR= include
OBJS= *.o

TEST_SRC= test/test*.c test/helpers.c
TEST_PROG= run_tests
PERF_TEST_SRC= test/perftest/perftest.c test/helpers.c
PERF_TEST_PROG= perftest

LIB_BASENAME= libcoll.so
LIB_SONAME= $(LIB_BASENAME).$(VER_MAJOR)
LIB_FILENAME= $(LIB_SONAME).$(VER_MINOR)

CFLAGS= -std=c99 -Wall -Wextra -pedantic -I$(INCLUDE_DIR)
CFLAGS_DEBUG= -DENABLE_DEBUG=1 -Og -g
CFLAGS_PROD= -O2
CFLAGS_LIB= -shared -fPIC
LDFLAGS_LIB= -shared

VALGRIND=valgrind
VALGRIND_OPTS= --leak-check=full --trace-children=yes

so:
	$(CC) $(CFLAGS) $(CFLAGS_LIB) $(CFLAGS_PROD) -c $(SRC)
	$(LD) $(LDFLAGS_LIB) -soname $(LIB_SONAME) -o $(LIB_FILENAME) -lc $(OBJS)
	ln -fs $(LIB_FILENAME) $(LIB_SONAME)
	ln -fs $(LIB_SONAME) $(LIB_BASENAME)

debug:
	$(CC) $(CFLAGS) $(CFLAGS_LIB) $(CFLAGS_DEBUG) -c $(SRC)
	$(LD) $(LDFLAGS_LIB) -soname $(LIB_SONAME) -o $(LIB_FILENAME) -lc $(OBJS)
	ln -fs $(LIB_FILENAME) $(LIB_SONAME)
	ln -fs $(LIB_SONAME) $(LIB_BASENAME)

tests: so
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(TEST_PROG) -L. -lcoll -lcheck

debugtests: debug
	$(CC) $(CFLAGS) $(TEST_SRC) $(CFLAGS_DEBUG) -o $(TEST_PROG) -L. -lcoll -lcheck

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

perftests: so
	$(CC) $(CFLAGS) $(CFLAGS_PROD) -o $(PERF_TEST_PROG) $(PERF_TEST_SRC) -L. -lcoll

runperftests: perftests
	@echo
	@echo Running performance tests...
	@echo
	LD_LIBRARY_PATH=. ./perftest hashmap
	@echo
	LD_LIBRARY_PATH=. ./perftest treemap

clean:
	rm -f $(OBJS) $(LIB_SONAME) $(LIB_FILENAME) $(LIB_BASENAME) $(TEST_PROG) $(PERF_TEST_PROG)

