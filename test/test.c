/*
 * Unit tests for the libdatastruct library.
 */

#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include "../linkedlist.h"
#include "../hashtable.h"
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
    printf("starting test linkedlist_populate_and_iterate");
    linkedlist_t *list = ll_init();

    int *testint1 = (int*) malloc(sizeof(int));
    int *testint2 = (int*) malloc(sizeof(int));

    *testint1 = 3;
    *testint2 = 5;

    ll_append(list, testint1);
    ll_append(list, testint2);

    ll_iter_t *iter = ll_get_iter(list);
    ck_assert(ll_has_next(iter));
    ck_assert_int_eq(*testint1, *((int*) ll_next(iter)->value));

    ck_assert(ll_has_next(iter));
    ck_assert_int_eq(*testint2, *((int*) ll_next(iter)->value));

    ll_drop_iter(iter);
    ll_deinit(list);
}
END_TEST


Suite *datastruct_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("datastruct");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, linkedlist_create);
    tcase_add_test(tc_core, linkedlist_populate_and_iterate);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = datastruct_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
