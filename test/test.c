/*
 * Unit tests for the libccoll library.
 */

#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include "../linkedlist.h"
#include "../hashmap.h"
#include "../treemap.h"


START_TEST(linkedlist_create)
{
    ccoll_linkedlist_t *ll = ccoll_linkedlist_init();
    ck_assert_ptr_nonnull(ll);
    ck_assert_uint_eq(ll->length, 0);
    ck_assert_ptr_null(ll->head);
    ck_assert_ptr_null(ll->tail);
    ccoll_linkedlist_deinit(ll);
}
END_TEST

START_TEST(linkedlist_populate_and_iterate)
{
    ccoll_linkedlist_t *list = ccoll_linkedlist_init();

    int *testint1 = (int*) malloc(sizeof(int));
    int *testint2 = (int*) malloc(sizeof(int));
    int *testint3 = (int*) malloc(sizeof(int));
    int *testint4 = (int*) malloc(sizeof(int));

    *testint1 = 2;
    *testint2 = 3;
    *testint3 = 5;
    *testint4 = 8;

    /* populate the list */
    ccoll_linkedlist_append(list, testint1);
    ccoll_linkedlist_append(list, testint2);
    ccoll_linkedlist_append(list, testint3);
    ccoll_linkedlist_append(list, testint4);
    ck_assert_uint_eq(ccoll_linkedlist_length(list), 4);
    ck_assert_int_eq(ccoll_linkedlist_index_of(list, testint3), 2);
    ck_assert(ccoll_linkedlist_contains(list, testint4));

    /* test iteration and retrieval */
    ccoll_linkedlist_iter_t *iter = ccoll_linkedlist_get_iter(list);
    ck_assert(ccoll_linkedlist_iter_has_next(iter));
    ck_assert_int_eq(*testint1, *((int*) ccoll_linkedlist_iter_next(iter)->value));

    ck_assert(ccoll_linkedlist_iter_has_next(iter));
    ck_assert_int_eq(*testint2, *((int*) ccoll_linkedlist_iter_next(iter)->value));

    ccoll_linkedlist_drop_iter(iter);

    /* test removal */
    char success = ccoll_linkedlist_remove(list, (void*) testint1);
    ck_assert_int_eq(success, 1);
    ck_assert_uint_eq(ccoll_linkedlist_length(list), 3);

    /* test iterator at */
    iter = ccoll_linkedlist_get_iter_at(list, 1);
    ck_assert(ccoll_linkedlist_iter_has_next(iter));
    ck_assert_int_eq(*testint3, *((int*) ccoll_linkedlist_iter_next(iter)->value));

    /* test removal through iterator */
    ccoll_linkedlist_iter_remove(iter);
    ck_assert_uint_eq(ccoll_linkedlist_length(list), 2);

    /* check that the last entry on the list is still accessible */
    ck_assert(ccoll_linkedlist_iter_has_next(iter));
    ck_assert_int_eq(*testint4, *((int*) ccoll_linkedlist_iter_next(iter)->value));

    /* should  be at the end of the list */
    ck_assert(!ccoll_linkedlist_iter_has_next(iter));

    ccoll_linkedlist_drop_iter(iter);

    ccoll_linkedlist_deinit(list);
}
END_TEST

START_TEST(hashmap_create)
{
    ccoll_hashmap_t *hashmap = ccoll_hashmap_init();
    ck_assert_ptr_nonnull(hashmap);
    ck_assert_ptr_nonnull(hashmap->hash_slots);
    ck_assert_ptr_nonnull(hashmap->key_comparator_function);
    ck_assert_ptr_nonnull(hashmap->hash_value_function);
    ck_assert_uint_ge(hashmap->capacity, 0);
    ck_assert_uint_eq(hashmap->total_entries, 0);
    
    ccoll_hashmap_deinit(hashmap);
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

TCase* create_hashmap_tests(void)
{
    TCase *tc_core;    
    tc_core = tcase_create("hashmap_core");
    
    tcase_add_test(tc_core, hashmap_create);
    
    return tc_core;
}

Suite* create_ccoll_test_suite(void)
{
    Suite *s;
    TCase *linkedlist_tests;
    TCase *hashmap_tests;
    
    s = suite_create("libccoll");
    linkedlist_tests = create_linkedlist_tests();
    hashmap_tests = create_hashmap_tests();
    
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

    s = create_ccoll_test_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
