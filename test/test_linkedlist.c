/*
 * Unit tests for the libcoll library.
 *
 * This file is part of libcoll, a generic collections library for C.
 *
 * Copyright (c) 2010-2020 Mika Wahlroos (mika.wahlroos@iki.fi)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <check.h>

#include "test_linkedlist.h"

#include "linkedlist.h"
#include "../src/debug.h"

/*
 * Tests that an empty linked list gets properly created.
 */
START_TEST(linkedlist_create)
{
    DEBUG("\n*** Starting linkedlist_create\n");
    libcoll_linkedlist_t *ll = libcoll_linkedlist_init();

    ck_assert_ptr_nonnull(ll);
    ck_assert_uint_eq(ll->length, 0);
    ck_assert_ptr_null(ll->head);
    ck_assert_ptr_null(ll->tail);
    libcoll_linkedlist_deinit(ll);
}
END_TEST

/*
 * Tests that appending values to a linked list works, that an iterator
 * properly returns the stored values, and that removing an item through
 * the iterator works.
 */
START_TEST(linkedlist_populate_and_iterate)
{
    DEBUG("\n*** Starting linkedlist_populate_and_iterate\n");
    libcoll_linkedlist_t *list = libcoll_linkedlist_init();

    int *testint1 = malloc(sizeof(int));
    int *testint2 = malloc(sizeof(int));
    int *testint3 = malloc(sizeof(int));
    int *testint4 = malloc(sizeof(int));

    *testint1 = 2;
    *testint2 = 3;
    *testint3 = 5;
    *testint4 = 8;

    /* populate the list */
    libcoll_linkedlist_append(list, testint1);
    libcoll_linkedlist_append(list, testint2);
    libcoll_linkedlist_append(list, testint3);
    libcoll_linkedlist_append(list, testint4);
    ck_assert_uint_eq(libcoll_linkedlist_length(list), 4);
    ck_assert_int_eq(libcoll_linkedlist_index_of(list, testint3), 2);
    ck_assert(libcoll_linkedlist_contains(list, testint4));

    /* test iteration and retrieval */
    libcoll_linkedlist_iter_t *iter = libcoll_linkedlist_get_iter(list);
    ck_assert(libcoll_linkedlist_iter_has_next(iter));
    ck_assert_int_eq(*testint1, *((int*) libcoll_linkedlist_iter_next(iter)));

    ck_assert(libcoll_linkedlist_iter_has_next(iter));
    ck_assert_int_eq(*testint2, *((int*) libcoll_linkedlist_iter_next(iter)));

    libcoll_linkedlist_drop_iter(iter);

    /* test removal */
    char success = libcoll_linkedlist_remove(list, (void*) testint1);
    ck_assert_int_eq(success, 1);
    ck_assert_uint_eq(libcoll_linkedlist_length(list), 3);

    /* test iterator at */
    iter = libcoll_linkedlist_get_iter_at(list, 1);
    ck_assert(libcoll_linkedlist_iter_has_next(iter));
    ck_assert_int_eq(*testint3, *((int*) libcoll_linkedlist_iter_next(iter)));

    /* test removal through iterator */
    libcoll_linkedlist_iter_remove(iter);
    ck_assert_uint_eq(libcoll_linkedlist_length(list), 2);

    /* check that the last entry on the list is still accessible */
    ck_assert(libcoll_linkedlist_iter_has_next(iter));
    ck_assert_int_eq(*testint4, *((int*) libcoll_linkedlist_iter_next(iter)));

    /* should now be at the end of the list */
    ck_assert(!libcoll_linkedlist_iter_has_next(iter));

    libcoll_linkedlist_drop_iter(iter);

    libcoll_linkedlist_deinit(list);

    free(testint1);
    free(testint2);
    free(testint3);
    free(testint4);
}
END_TEST

TCase* create_linkedlist_tests(void)
{
    TCase *tc_core;

    /* Core test case */
    tc_core = tcase_create("linkedlist_core");

    tcase_add_test(tc_core, linkedlist_create);
    tcase_add_test(tc_core, linkedlist_populate_and_iterate);

    return tc_core;
}