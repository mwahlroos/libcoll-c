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

#include "test_hashmap.h"
#include "test_linkedlist.h"
#include "test_treemap.h"
#include "test_vector.h"
#include "helpers.h"

#include "comparators.h"  /* for the comparator sanity tests */

#include "../src/debug.h"


START_TEST(comparator_self_sanity_check)
{
    int a, b, c;
    a = b = 1;
    c = 2;

    ck_assert_int_eq(libcoll_intptrcmp(&a, &b), 0);
    ck_assert_int_lt(libcoll_intptrcmp(&a, &c), 0);
}
END_TEST

TCase* create_self_sanity_test(void)
{
    TCase *tc_core;
    tc_core = tcase_create("self_sanity_core");
    tcase_add_test(tc_core, comparator_self_sanity_check);
    return tc_core;
}

Suite* create_libcoll_test_suite(void)
{
    Suite *s;
    TCase *linkedlist_tests;
    TCase *vector_tests;
    TCase *hashmap_tests;
    TCase *treemap_tests;
    TCase *self_sanity_test;

    s = suite_create("libcoll");
    linkedlist_tests = create_linkedlist_tests();
    vector_tests = create_vector_tests();
    hashmap_tests = create_hashmap_tests();
    treemap_tests = create_treemap_tests();
    self_sanity_test = create_self_sanity_test();

    suite_add_tcase(s, self_sanity_test);
    suite_add_tcase(s, linkedlist_tests);
    suite_add_tcase(s, vector_tests);
    suite_add_tcase(s, hashmap_tests);
    suite_add_tcase(s, treemap_tests);

    return s;
}

int main(void)
{
    /* basic non-interactive test runner, code adapted from the
     * check library documentation
     */

    int number_failed;
    Suite *s;
    SRunner *sr;

    s = create_libcoll_test_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
