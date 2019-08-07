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
    linkedlist_t *ll = ll_init();
    ck_assert_ptr_nonnull(ll);
    ck_assert_uint_eq(ll->length, 0);
    ck_assert_ptr_null(ll->head);
    ck_assert_ptr_null(ll->tail);
    ll_deinit(ll);
}
END_TEST

START_TEST(linkedlist_populate_and_iterate)
{
    linkedlist_t *list = ll_init();

    int *testint1 = (int*) malloc(sizeof(int));
    int *testint2 = (int*) malloc(sizeof(int));
    int *testint3 = (int*) malloc(sizeof(int));
    int *testint4 = (int*) malloc(sizeof(int));

    *testint1 = 2;
    *testint2 = 3;
    *testint3 = 5;
    *testint4 = 8;

    /* populate the list */
    ll_append(list, testint1);
    ll_append(list, testint2);
    ll_append(list, testint3);
    ll_append(list, testint4);
    ck_assert_uint_eq(ll_length(list), 4);
    ck_assert_int_eq(ll_index_of(list, testint3), 2);
    ck_assert(ll_contains(list, testint4));

    /* test iteration and retrieval */
    ll_iter_t *iter = ll_get_iter(list);
    ck_assert(ll_iter_has_next(iter));
    ck_assert_int_eq(*testint1, *((int*) ll_iter_next(iter)->value));

    ck_assert(ll_iter_has_next(iter));
    ck_assert_int_eq(*testint2, *((int*) ll_iter_next(iter)->value));

    ll_drop_iter(iter);

    /* test removal */
    char success = ll_remove(list, (void*) testint1);
    ck_assert_int_eq(success, 1);
    ck_assert_uint_eq(ll_length(list), 3);

    /* test iterator at */
    iter = ll_get_iter_at(list, 1);
    ck_assert(ll_iter_has_next(iter));
    ck_assert_int_eq(*testint3, *((int*) ll_iter_next(iter)->value));

    /* test removal through iterator */
    ll_iter_remove(iter);
    ck_assert_uint_eq(ll_length(list), 2);

    /* check that the last entry on the list is still accessible */
    ck_assert(ll_iter_has_next(iter));
    ck_assert_int_eq(*testint4, *((int*) ll_iter_next(iter)->value));

    /* should  be at the end of the list */
    ck_assert(!ll_iter_has_next(iter));

    ll_drop_iter(iter);

    ll_deinit(list);
}
END_TEST


Suite *ccoll_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("ccoll");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, linkedlist_create);
    tcase_add_test(tc_core, linkedlist_populate_and_iterate);
    suite_add_tcase(s, tc_core);

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

    s = ccoll_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
