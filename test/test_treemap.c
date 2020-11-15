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

#include "test_treemap.h"

#include "comparators.h"
#include "treemap.h"
#include "types.h"

#include "../src/debug.h"

/* test data, consisting of imaginary string occurrence counts */
libcoll_treemap_t *string_counts;

static const size_t TEST_KEY_COUNT = 10;

static char *string_counts_keys[] = { "axe", "asdf", "bar", "foo", "alter ego", "quine",
                                      "ra", "rust", "rendezvous", "xylophone" };
static int string_counts_values[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

/* test fixtures, run once per test */

void _setup_string_counts_treemap(void)
{
    DEBUG("\n*** Setting up a treemap for testing\n");
    string_counts = libcoll_treemap_init_with_comparator(libcoll_strcmp_wrapper);

    /*
     * Allocate and insert keys and values.
     *
     * malloc()ing the keys and values wouldn't really be necessary here and
     * inserting stack-allocated ones would do okay in this test case, but
     * duplicating the keys and values allows for testing that key comparison
     * using the custom comparator works properly.
     */
    for (size_t i=0; i<TEST_KEY_COUNT; i++) {
        char *key = malloc((strlen(string_counts_keys[i]) + 1) * sizeof(char));
        strcpy(key, string_counts_keys[i]);
        int *value = malloc(sizeof(int));

        *value = string_counts_values[i];
        libcoll_treemap_add(string_counts, key, value);
    }
}

void _teardown_string_counts_treemap(void)
{
    /* clean up by freeing any remaining keys and values */
    libcoll_treemap_iter_t *iter = libcoll_treemap_get_iterator(string_counts);

    while (libcoll_treemap_has_next(iter)) {
        libcoll_treemap_next(iter);
        libcoll_pair_voidptr_t entry = libcoll_treemap_remove_last_traversed(iter);
        free(entry.a);
        free(entry.b);
    }

    libcoll_treemap_drop_iterator(iter);

    libcoll_treemap_deinit(string_counts);
}

/*
 * Tests that an empty treemap gets created correctly.
 */
START_TEST(treemap_create)
{
    DEBUG("\n*** Starting treemap_create\n");
    libcoll_treemap_t *treemap = libcoll_treemap_init();

    ck_assert_ptr_nonnull(treemap);
    ck_assert_uint_eq(treemap->size, 0);

    libcoll_treemap_deinit(treemap);
}
END_TEST

/*
 * Tests that a populated treemap contains and retrieves expected values
 * correctly, and that key/value pairs get properly removed.
 */
START_TEST(treemap_retrieve_and_remove)
{
    DEBUG("\n*** Starting treemap_retrieve_and_remove\n");

    size_t member_count = libcoll_treemap_get_size(string_counts);

    ck_assert_uint_eq(member_count, TEST_KEY_COUNT);

    /* test retrieving a nonexisting value */
    char *invalid_key = "xkcd";
    ck_assert(!libcoll_treemap_contains(string_counts, invalid_key));
    ck_assert_ptr_null(libcoll_treemap_get(string_counts, invalid_key));

    /* test retrieving and removing valid values */
    for (size_t i=0; i<TEST_KEY_COUNT; i++) {
        char *key = string_counts_keys[i];
        int expected_value = string_counts_values[i];

        ck_assert(libcoll_treemap_contains(string_counts, key));

        libcoll_treemap_node_t *node = libcoll_treemap_get(string_counts, key);
        int *retrieved_value = node->value;

        DEBUGF("Treemap: retrieved %d with key %s; expected %d\n",
               *retrieved_value, key, expected_value
        );

        ck_assert_ptr_nonnull(retrieved_value);
        ck_assert_int_eq(*retrieved_value, expected_value);

        libcoll_pair_voidptr_t entry = libcoll_treemap_remove(string_counts, key);
        ck_assert_str_eq(key, (char*) entry.a);
        ck_assert_int_eq(expected_value, *(int*) entry.b);

        free(entry.a);
        free(entry.b);

        ck_assert_uint_eq(libcoll_treemap_get_size(string_counts), --member_count);
    }
}
END_TEST

/*
 * Tests that the treemap iterator returns all expected keys and in expected
 * (sorted) order.
 */
START_TEST(treemap_iterate)
{
    DEBUG("\n*** Starting treemap_iterate\n");

    libcoll_treemap_iter_t *iter = libcoll_treemap_get_iterator(string_counts);
    char *last_key = NULL;
    for (size_t i=0; i<TEST_KEY_COUNT; i++) {
        ck_assert(libcoll_treemap_has_next(iter));

        libcoll_treemap_node_t *node = libcoll_treemap_next(iter);
        char *retrieved_key = (char*) node->key;
        if (last_key) {
            ck_assert_str_lt(last_key, retrieved_key);
        }
        last_key = retrieved_key;
    }

    ck_assert(!libcoll_treemap_has_next(iter));

    libcoll_treemap_drop_iterator(iter);
}

TCase* create_treemap_tests(void)
{
    TCase *tc_core;
    tc_core = tcase_create("treemap_core");

    tcase_add_checked_fixture(tc_core,
                              _setup_string_counts_treemap,
                              _teardown_string_counts_treemap);

    tcase_add_test(tc_core, treemap_create);
    tcase_add_test(tc_core, treemap_retrieve_and_remove);
    tcase_add_test(tc_core, treemap_iterate);

    return tc_core;
}
