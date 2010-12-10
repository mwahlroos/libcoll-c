/*
 * linkedlist_test.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"
#include "node.h"

int intcmp(node_value_t *node_a, node_value_t *node_b)
{
    int *a = (int*) (node_a->payload);
    int *b = (int*) (node_b->payload);
    int cmpval;
    if (*a < *b) {
        cmpval = -1;
    } else if (*a > *b) {
        cmpval = 1;
    } else {
        cmpval = 0;
    }
    return cmpval;
}

void print_int_list(linkedlist_t *list)
{
    printf("List length: %d\n", list->length);

    ll_iter_t *iter = ll_get_iter(list);
    ll_node_t *node;
    int counter = 0;
    while (ll_has_next(iter)) {
        node = ll_next(iter);
        int *intval = (int*) node->value->payload;
        printf("%d:\t%d\t(at %u)\n", counter, *intval, (unsigned) intval);
        counter++;
    }
    printf("\n");
    ll_drop_iter(iter);
}

void add_int_to_list(linkedlist_t *list, int value)
{
    int *value_ptr = (int*) malloc (sizeof(int));
    *value_ptr = value;
    node_value_t *nodeval = (node_value_t*) malloc (sizeof(node_value_t));
    nodeval->payload = (void*) value_ptr;
    ll_append(list, nodeval);
}

int ll_test_main()
{
    linkedlist_t *list1 = ll_init(&intcmp);

    int arrlen = 5;
    int intarr[] = { 2, 3, 5, 7, 11 };
    for (int i=0; i<arrlen; ++i) {
        printf("Trying to insert %d\n", intarr[i]);
        add_int_to_list(list1, intarr[i]);
        print_int_list(list1);
    }

    ll_deinit(list1);
    return EXIT_SUCCESS;
}

