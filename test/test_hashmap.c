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
#include "test_hashmap.h"

#include "comparators.h"
#include "hash.h"
#include "hashmap.h"
#include "vector.h"  /* use as a utility type */

#include "../src/debug.h"

START_TEST(hashmap_create)
{
    DEBUG("\n*** Starting hashmap_create\n");
    libcoll_hashmap_t *hashmap = libcoll_hashmap_init();
    ck_assert_ptr_nonnull(hashmap);
    ck_assert_ptr_nonnull(hashmap->buckets);
    ck_assert(hashmap->key_comparator_function != NULL);
    ck_assert(hashmap->hash_code_function != NULL);
    ck_assert_uint_ge(hashmap->capacity, 0);
    ck_assert_uint_eq(hashmap->total_entries, 0);

    libcoll_hashmap_deinit(hashmap);
}
END_TEST

START_TEST(hashmap_populate_and_retrieve)
{
    DEBUG("\n*** Starting hashmap_populate_and_retrieve\n");
    libcoll_hashmap_t *counts = libcoll_hashmap_init();
    counts->hash_code_function = hashcode_str;
    counts->key_comparator_function = libcoll_strcmp_wrapper;

    char *identifiers[] = { "identifier_1", "identifier_2" };
    int values[] = { 7, -34 };

    /* test inserting values */
    for (int i=0; i<2; i++) {
        char *key = malloc((strlen(identifiers[i]) + 1) * sizeof(char));
        strcpy(key, identifiers[i]);
        int *value = malloc(sizeof(int));

        *value = values[i];
        libcoll_hashmap_put(counts, key, value);
    }

    ck_assert_uint_eq(libcoll_hashmap_get_size(counts), 2);
    ck_assert(libcoll_hashmap_contains(counts, identifiers[0]));

    size_t member_count = libcoll_hashmap_get_size(counts);

    /* test retrieving a nonexisting value */
    char *invalid_key = "asdf";
    ck_assert_ptr_null(libcoll_hashmap_get(counts, invalid_key));
    ck_assert_uint_eq(libcoll_hashmap_get_size(counts), member_count);

    /* test retrieving and removing valid values */
    for (int i=0; i<2; i++) {
        char *key = identifiers[i];
        int expected_value = values[i];

        ck_assert(libcoll_hashmap_contains(counts, key));

        int *retrieved_value = (int*) libcoll_hashmap_get(counts, key);

        DEBUGF("Hashmap: retrieved %d with key %s; expected %d\n",
               *retrieved_value, key, expected_value
        );

        ck_assert_ptr_nonnull(retrieved_value);
        ck_assert_int_eq(*retrieved_value, expected_value);

        libcoll_map_removal_result_t result = libcoll_hashmap_remove(counts, key);
        ck_assert_str_eq(key, (char*) result.key);
        ck_assert_int_eq(expected_value, *(int*) result.value);

        free(result.key);
        free(result.value);

        ck_assert_uint_eq(libcoll_hashmap_get_size(counts), --member_count);
    }

    libcoll_hashmap_deinit(counts);
}
END_TEST

START_TEST(hashmap_iterate)
{
    DEBUG("\n*** Starting hashmap_iterate\n");
    libcoll_hashmap_t *hm = libcoll_hashmap_init();
    hm->hash_code_function = hashcode_str;
    hm->key_comparator_function = libcoll_strcmp_wrapper;

    size_t test_value_count = 3;

    char *keys[] = { "key1", "key2", "key3" };
    int values[] = { 3, 4, 5 };

    /* put the values in a vector so that the values retrieved
     * from the hashmap can later be compared against them
     */
    libcoll_vector_t *tmpvect = libcoll_vector_init();
    tmpvect->compare_function = libcoll_intptrcmp;

    for (size_t i=0; i<test_value_count; i++) {
        libcoll_vector_append(tmpvect, &values[i]);
    }

    /* sanity check to make sure at least the vector contains all values */
    ck_assert_uint_eq(libcoll_vector_length(tmpvect), 3);

    for (size_t i=0; i<test_value_count; i++) {
        libcoll_hashmap_put(hm, keys[i], &values[i]);
    }

    ck_assert_uint_eq(libcoll_hashmap_get_size(hm), 3);

    libcoll_hashmap_iter_t *iter = libcoll_hashmap_get_iterator(hm);
    for (int i=0; i<3; i++) {
        ck_assert(libcoll_hashmap_iter_has_next(iter));

        libcoll_hashmap_entry_t *entry = libcoll_hashmap_iter_next(iter);
        int *value = (int*) entry->value;

        ck_assert(libcoll_vector_contains(tmpvect, value));

        libcoll_vector_remove(tmpvect, value);
    }

    ck_assert(!libcoll_hashmap_iter_has_next(iter));

    libcoll_hashmap_drop_iterator(iter);
    libcoll_vector_deinit(tmpvect);
    libcoll_hashmap_deinit(hm);
}

START_TEST(hashmap_resize)
{
    /* test hashmap capacity increase and the associated rehashing */
    DEBUG("\n*** Starting hashmap_resize\n");
    size_t init_capacity = 2LU;
    size_t previous_capacity;

    float max_load_factor = 0.6f;
    DEBUGF("Initializing hashmap with a capacity=%lu, max_load_factor=%f\n",
          init_capacity, max_load_factor
    );
    libcoll_hashmap_t *hm = libcoll_hashmap_init_with_params(
            init_capacity,
            max_load_factor,
            hashcode_int,
            libcoll_intptrcmp,
            libcoll_strcmp_wrapper
    );

    int *testkey1 = (int*) malloc(sizeof(int));
    int *testkey2 = (int*) malloc(sizeof(int));
    int *testkey3 = (int*) malloc(sizeof(int));
    int *testkey4 = (int*) malloc(sizeof(int));
    *testkey1 = 2;
    *testkey2 = 3;
    *testkey3 = 5;
    *testkey4 = 8;

    /* Using stack-allocated values is technically wrong but works here */
    char *testval1 = "foo";
    char *testval2 = "bar";
    char *testval3 = "baz";
    char *testval4 = "quux";

    print_hashmap(hm);

    ck_assert_uint_eq(hm->capacity, init_capacity);

    DEBUGF("Inserting key-value pair #1 ([%p] %d -> [%p] %s)...\n",
           (void*) testkey1, *testkey1, (void*) testval1, testval1
    );
    libcoll_hashmap_put(hm, testkey1, testval1);
    print_hashmap(hm);

    ck_assert_uint_eq(hm->capacity, init_capacity);

    DEBUGF("Inserting key-value pair with existing key ([%p] %d -> [%p] -> %s)...\n",
          (void*) testkey1, *testkey1, (void*) testval2, testval2
    );

    libcoll_hashmap_put(hm, testkey1, testval2);
    print_hashmap(hm);

    ck_assert_uint_eq(hm->capacity, init_capacity);
    DEBUGF("testval2 == %s, testkey1 == %d, val(testkey1) == %s\n", testval2, *testkey1, (char*) libcoll_hashmap_get(hm, testkey1));
    ck_assert_str_eq(testval2, (char*) libcoll_hashmap_get(hm, testkey1));

    DEBUGF("Inserting key-value pair #2 ([%p] %d -> [%p] %s)...\n",
           (void*) testkey2, *testkey2, (void*) testval2, testval2
    );
    libcoll_hashmap_put(hm, testkey2, testval2);
    print_hashmap(hm);

    /* the capacity should have been increased by now from the initial 2 */
    ck_assert_uint_gt(hm->capacity, init_capacity);

    ck_assert(libcoll_hashmap_contains(hm, testkey1));
    ck_assert_str_eq(testval2, (char*) libcoll_hashmap_get(hm, testkey1));

    previous_capacity = hm->capacity;

    DEBUG("Inserting key-value pair #3...\n");
    libcoll_hashmap_put(hm, testkey3, testval3);
    print_hashmap(hm);

    ck_assert_uint_gt(hm->capacity, previous_capacity);
    ck_assert_str_eq(testval2, (char*) libcoll_hashmap_get(hm, testkey1)); 

    previous_capacity = hm->capacity;

    DEBUG("Inserting key-value pair #4...\n");
    libcoll_hashmap_put(hm, testkey4, testval4);
    print_hashmap(hm);

    ck_assert_uint_eq(hm->capacity, previous_capacity);

    /* check retrieval after resizings */
    ck_assert_str_eq(testval2, (char*) libcoll_hashmap_get(hm, testkey1));
    ck_assert_str_eq(testval2, (char*) libcoll_hashmap_get(hm, testkey2));
    ck_assert_str_eq(testval3, (char*) libcoll_hashmap_get(hm, testkey3));
    ck_assert_str_eq(testval4, (char*) libcoll_hashmap_get(hm, testkey4));

    /* free all resources */
    libcoll_hashmap_deinit(hm);

    free(testkey1);
    free(testkey2);
    free(testkey3);
    free(testkey4);
}
END_TEST

TCase* create_hashmap_tests(void)
{
    TCase *tc_core;
    tc_core = tcase_create("hashmap_core");

    tcase_add_test(tc_core, hashmap_create);
    tcase_add_test(tc_core, hashmap_populate_and_retrieve);
    tcase_add_test(tc_core, hashmap_iterate);
    tcase_add_test(tc_core, hashmap_resize);

    return tc_core;
}
