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
    ccoll_linkedlist_t *ll = ccoll_ll_init();
    ck_assert_ptr_nonnull(ll);
    ck_assert_uint_eq(ll->length, 0);
    ck_assert_ptr_null(ll->head);
    ck_assert_ptr_null(ll->tail);
    ccoll_ll_deinit(ll);
}
END_TEST

START_TEST(linkedlist_populate_and_iterate)
{
    ccoll_linkedlist_t *list = ccoll_ll_init();

    int *testint1 = (int*) malloc(sizeof(int));
    int *testint2 = (int*) malloc(sizeof(int));
    int *testint3 = (int*) malloc(sizeof(int));
    int *testint4 = (int*) malloc(sizeof(int));

    *testint1 = 2;
    *testint2 = 3;
    *testint3 = 5;
    *testint4 = 8;

    /* populate the list */
    ccoll_ll_append(list, testint1);
    ccoll_ll_append(list, testint2);
    ccoll_ll_append(list, testint3);
    ccoll_ll_append(list, testint4);
    ck_assert_uint_eq(ccoll_ll_length(list), 4);
    ck_assert_int_eq(ccoll_ll_index_of(list, testint3), 2);
    ck_assert(ccoll_ll_contains(list, testint4));

    /* test iteration and retrieval */
    ccoll_ll_iter_t *iter = ccoll_ll_get_iter(list);
    ck_assert(ccoll_ll_iter_has_next(iter));
    ck_assert_int_eq(*testint1, *((int*) ccoll_ll_iter_next(iter)->value));

    ck_assert(ccoll_ll_iter_has_next(iter));
    ck_assert_int_eq(*testint2, *((int*) ccoll_ll_iter_next(iter)->value));

    ccoll_ll_drop_iter(iter);

    /* test removal */
    char success = ccoll_ll_remove(list, (void*) testint1);
    ck_assert_int_eq(success, 1);
    ck_assert_uint_eq(ccoll_ll_length(list), 3);

    /* test iterator at */
    iter = ccoll_ll_get_iter_at(list, 1);
    ck_assert(ccoll_ll_iter_has_next(iter));
    ck_assert_int_eq(*testint3, *((int*) ccoll_ll_iter_next(iter)->value));

    /* test removal through iterator */
    ccoll_ll_iter_remove(iter);
    ck_assert_uint_eq(ccoll_ll_length(list), 2);

    /* check that the last entry on the list is still accessible */
    ck_assert(ccoll_ll_iter_has_next(iter));
    ck_assert_int_eq(*testint4, *((int*) ccoll_ll_iter_next(iter)->value));

    /* should  be at the end of the list */
    ck_assert(!ccoll_ll_iter_has_next(iter));

    ccoll_ll_drop_iter(iter);

    ccoll_ll_deinit(list);
}
END_TEST


Suite *ccoccoll_ll_suite(void)
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

    s = ccoccoll_ll_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
