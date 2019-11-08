/*
 * Unit tests for the libcoll library.
 */

#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../linkedlist.h"
#include "../hashmap.h"
#include "../treemap.h"
#include "../map.h"
#include "../node.h"
#include "../types.h"
#include "../hash.h"
#include "../debug.h"

/* helper functions */

/* Compares two pointers by the integer value they point to.
 * Utility function for unit tests.
 */
int intptrcmp(void *value1, void *value2)
{
    int *a = (int*) value1;
    int *b = (int*) value2;
    return *a - *b;
}

/* Compares two pointers by their string values.
 * Wrapper around strcmp that accepts void pointers, for comparing map keys
 * and/or values.
 */
int strcmp_wrapper(void *value1, void *value2)
{
    char *s1 = (char*) value1;
    char *s2 = (char*) value2;

    return strcmp(s1, s2);
}

static void print_hashmap(libcoll_hashmap_t *hm)
{
    for (size_t i=0; i<hm->capacity; i++) {
        libcoll_linkedlist_t *collision_list = hm->buckets[i];
        if (NULL == collision_list) {
            DEBUGF("[%lu] empty bucket\n", i);
        } else {
            DEBUGF("[%lu] nonempty bucket:", i);
            libcoll_linkedlist_iter_t *iter = libcoll_linkedlist_get_iter(collision_list);
            while (libcoll_linkedlist_iter_has_next(iter)) {
                libcoll_linkedlist_node_t *node = libcoll_linkedlist_iter_next(iter);
                libcoll_hashmap_entry_t *entry = (libcoll_hashmap_entry_t*) node->value;
                DEBUGF(" (%p -> %p)", entry->key, entry->value);
                DEBUG("\n");
            }

            libcoll_linkedlist_drop_iter(iter);
        }
    }
}

/* actual tests */

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

START_TEST(linkedlist_populate_and_iterate)
{
    DEBUG("\n*** Starting linkedlist_populate_and_iterate\n");
    libcoll_linkedlist_t *list = libcoll_linkedlist_init();

    int *testint1 = (int*) malloc(sizeof(int));
    int *testint2 = (int*) malloc(sizeof(int));
    int *testint3 = (int*) malloc(sizeof(int));
    int *testint4 = (int*) malloc(sizeof(int));

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
    ck_assert_int_eq(*testint1, *((int*) libcoll_linkedlist_iter_next(iter)->value));

    ck_assert(libcoll_linkedlist_iter_has_next(iter));
    ck_assert_int_eq(*testint2, *((int*) libcoll_linkedlist_iter_next(iter)->value));

    libcoll_linkedlist_drop_iter(iter);

    /* test removal */
    char success = libcoll_linkedlist_remove(list, (void*) testint1);
    ck_assert_int_eq(success, 1);
    ck_assert_uint_eq(libcoll_linkedlist_length(list), 3);

    /* test iterator at */
    iter = libcoll_linkedlist_get_iter_at(list, 1);
    ck_assert(libcoll_linkedlist_iter_has_next(iter));
    ck_assert_int_eq(*testint3, *((int*) libcoll_linkedlist_iter_next(iter)->value));

    /* test removal through iterator */
    libcoll_linkedlist_iter_remove(iter);
    ck_assert_uint_eq(libcoll_linkedlist_length(list), 2);

    /* check that the last entry on the list is still accessible */
    ck_assert(libcoll_linkedlist_iter_has_next(iter));
    ck_assert_int_eq(*testint4, *((int*) libcoll_linkedlist_iter_next(iter)->value));

    /* should  be at the end of the list */
    ck_assert(!libcoll_linkedlist_iter_has_next(iter));

    libcoll_linkedlist_drop_iter(iter);

    libcoll_linkedlist_deinit(list);

    free(testint1);
    free(testint2);
    free(testint3);
    free(testint4);
}
END_TEST

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
    counts->hash_code_function = hashcode_str2;
    counts->key_comparator_function = strcmp_wrapper;

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
            intptrcmp,
            strcmp_wrapper
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

    DEBUGF("Hashmap contents/capacity: %lu/%lu\n",
          libcoll_hashmap_get_size(hm),
          libcoll_hashmap_get_capacity(hm)
    );
    DEBUG("Hashmap contents after insertion:\n");
    print_hashmap(hm);

    ck_assert_uint_eq(hm->capacity, init_capacity);

    DEBUGF("Inserting key-value pair #1 ([%p] %d -> [%p] %s)...\n",
           (void*) testkey1, *testkey1, (void*) testval1, testval1
    );
    libcoll_hashmap_put(hm, testkey1, testval1);
    DEBUGF("Hashmap contents/capacity: %lu/%lu\n",
          libcoll_hashmap_get_size(hm),
          libcoll_hashmap_get_capacity(hm)
    );
    DEBUG("Hashmap contents after insertion:\n");
    print_hashmap(hm);

    ck_assert_uint_eq(hm->capacity, init_capacity);

    DEBUGF("Inserting key-value pair with existing key ([%p] %d -> [%p] -> %s)...\n",
          (void*) testkey1, *testkey1, (void*) testval2, testval2
    );

    libcoll_hashmap_put(hm, testkey1, testval2);
    DEBUGF("Hashmap contents/capacity: %lu/%lu\n",
          libcoll_hashmap_get_size(hm),
          libcoll_hashmap_get_capacity(hm)
    );
    DEBUG("Hashmap contents after insertion:\n");
    print_hashmap(hm);

    ck_assert_uint_eq(hm->capacity, init_capacity);
    ck_assert_str_eq(testval2, (char*) libcoll_hashmap_get(hm, testkey1));

    DEBUGF("Inserting key-value pair #2 ([%p] %d -> [%p] %s)...\n",
           (void*) testkey2, *testkey2, (void*) testval2, testval2
    );
    libcoll_hashmap_put(hm, testkey2, testval2);
    DEBUGF("Hashmap contents/capacity: %lu/%lu\n",
          libcoll_hashmap_get_size(hm),
          libcoll_hashmap_get_capacity(hm)
    );
    DEBUG("Hashmap contents after insertion:\n");
    print_hashmap(hm);

    /* the capacity should have been increased by now from the initial 2 */
    ck_assert_uint_gt(hm->capacity, init_capacity);

    ck_assert(libcoll_hashmap_contains(hm, testkey1));
    ck_assert_str_eq(testval2, (char*) libcoll_hashmap_get(hm, testkey1));

    previous_capacity = hm->capacity;

    DEBUG("Inserting key-value pair #3...\n");
    libcoll_hashmap_put(hm, testkey3, testval3);
    DEBUGF("Hashmap contents/capacity: %lu/%lu\n",
        libcoll_hashmap_get_size(hm),
        libcoll_hashmap_get_capacity(hm)
    );
    DEBUG("Hashmap contents after insertion:\n");
    print_hashmap(hm);

    ck_assert_uint_gt(hm->capacity, previous_capacity);
    ck_assert_str_eq(testval2, (char*) libcoll_hashmap_get(hm, testkey1)); 

    previous_capacity = hm->capacity;

    DEBUG("Inserting key-value pair #4...\n");
    libcoll_hashmap_put(hm, testkey4, testval4);
    DEBUGF("Hashmap contents/capacity: %lu/%lu\n",
        libcoll_hashmap_get_size(hm),
        libcoll_hashmap_get_capacity(hm)
    );
    DEBUG("Hashmap contents after insertion:\n");
    print_hashmap(hm);

    ck_assert_uint_eq(hm->capacity, previous_capacity);

    /* check retrieval after resizings */
    ck_assert_str_eq(testval2, (char*) libcoll_hashmap_get(hm, testkey1));

    /* free all resources */
    libcoll_hashmap_deinit(hm);

    free(testkey1);
    free(testkey2);
    free(testkey3);
    free(testkey4);
}
END_TEST

START_TEST(comparator_self_sanity_check)
{
    int a, b, c;
    a = b = 1;
    c = 2;

    ck_assert_int_eq(intptrcmp(&a, &b), 0);
    ck_assert_int_lt(intptrcmp(&a, &c), 0);
}
END_TEST

TCase* create_self_sanity_test(void)
{
    TCase *tc_core;
    tc_core = tcase_create("self_sanity_core");
    tcase_add_test(tc_core, comparator_self_sanity_check);
    return tc_core;
}

TCase* create_linkedlist_tests(void)
{
    TCase *tc_core;

    /* Core test case */
    tc_core = tcase_create("linkedlist_core");

    tcase_add_test(tc_core, linkedlist_create);
    tcase_add_test(tc_core, linkedlist_populate_and_iterate);

    return tc_core;
}

TCase* create_hashmap_tests(void)
{
    TCase *tc_core;
    tc_core = tcase_create("hashmap_core");

    tcase_add_test(tc_core, hashmap_create);
    tcase_add_test(tc_core, hashmap_populate_and_retrieve);
    tcase_add_test(tc_core, hashmap_resize);

    return tc_core;
}

Suite* create_libcoll_test_suite(void)
{
    Suite *s;
    TCase *linkedlist_tests;
    TCase *hashmap_tests;
    TCase *self_sanity_test;

    s = suite_create("libcoll");
    linkedlist_tests = create_linkedlist_tests();
    hashmap_tests = create_hashmap_tests();
    self_sanity_test = create_self_sanity_test();

    suite_add_tcase(s, self_sanity_test);
    suite_add_tcase(s, linkedlist_tests);
    suite_add_tcase(s, hashmap_tests);

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
