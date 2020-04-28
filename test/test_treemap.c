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

#include "helpers.h"
#include "test_treemap.h"

#include "treemap.h"
#include "types.h"

#include "../src/debug.h"

START_TEST(treemap_create)
{
    DEBUG("\n*** Starting treemap_create\n");
    libcoll_treemap_t *treemap = libcoll_treemap_init();
    ck_assert_ptr_nonnull(treemap);
    ck_assert_uint_eq(treemap->size, 0);

    libcoll_treemap_deinit(treemap);
}
END_TEST

START_TEST(treemap_populate_and_retrieve)
{
    DEBUG("\n*** Starting treemap_populate_and_retrieve\n");
    libcoll_treemap_t *counts = libcoll_treemap_init();
    counts->key_comparator = strcmp_wrapper;

    const size_t test_key_count = 10;

    char *keys[] = { "axe", "asdf", "bar", "foo", "alter ego", "quine",
                   "ra", "rust", "rendezvous", "xylophone" };

    int values[test_key_count];
    for (size_t i=0; i<test_key_count; i++) {
        values[i] = (int) i;
    }

    /*
     * Generate values to insert.
     *
     * malloc()ing the keys and values wouldn't really be necessary here and
     * inserting stack-allocated ones would do okay in this test case, but
     * duplicating the keys and values allows for testing that key comparison
     * using the custom comparator works properly.
     */
    for (size_t i=0; i<test_key_count; i++) {
        char *key = malloc((strlen(keys[i]) + 1) * sizeof(char));
        strcpy(key, keys[i]);
        int *value = malloc(sizeof(int));

        *value = values[i];
        libcoll_treemap_add(counts, key, value);
    }

    size_t member_count = libcoll_treemap_get_size(counts);

    ck_assert_uint_eq(member_count, test_key_count);

    /* test retrieving a nonexisting value */
    char *invalid_key = "xkcd";
    ck_assert(!libcoll_treemap_contains(counts, invalid_key));
    ck_assert_ptr_null(libcoll_treemap_get(counts, invalid_key));

    /* test retrieving and removing valid values */
    for (size_t i=0; i<test_key_count; i++) {
        char *key = keys[i];
        int expected_value = values[i];

        ck_assert(libcoll_treemap_contains(counts, key));

        libcoll_treemap_node_t *node = libcoll_treemap_get(counts, key);
        int *retrieved_value = node->value;

        DEBUGF("Hashmap: retrieved %d with key %s; expected %d\n",
               *retrieved_value, key, expected_value
        );

        ck_assert_ptr_nonnull(retrieved_value);
        ck_assert_int_eq(*retrieved_value, expected_value);

        libcoll_pair_voidptr_t entry = libcoll_treemap_remove(counts, key);
        ck_assert_str_eq(key, (char*) entry.a);
        ck_assert_int_eq(expected_value, *(int*) entry.b);

        free(entry.a);
        free(entry.b);

        ck_assert_uint_eq(libcoll_treemap_get_size(counts), --member_count);
    }

    libcoll_treemap_deinit(counts);
}
END_TEST

TCase* create_treemap_tests(void)
{
    TCase *tc_core;
    tc_core = tcase_create("treemap_core");

    tcase_add_test(tc_core, treemap_create);
    tcase_add_test(tc_core, treemap_populate_and_retrieve);

    return tc_core;
}
