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

void print_node_as_int_with_index(ll_node_t *node, int index)
{
    int *intval = (int*) node->value->payload;
    if (index >= 0) {
        printf("%d ", index);
    }
    printf("(node at 0x%x):\t%d (at 0x%x)\n", (unsigned) node, *intval, (unsigned) intval);
}

void print_node_as_int(ll_node_t *node)
{
    print_node_as_int_with_index(node, -1);
}

void print_int_list(linkedlist_t *list)
{
    printf("List length: %d\n", list->length);
    printf("Head: ");
    print_node_as_int(list->head);
    printf("Tail: ");
    print_node_as_int(list->tail);

    ll_iter_t *iter = ll_get_iter(list);
    ll_node_t *node;
    int counter = 0;
    while (ll_has_next(iter)) {
        node = ll_next(iter);
        print_node_as_int_with_index(node, counter);
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

void test_iter_at(linkedlist_t *list, size_t init_index)
{
    ll_iter_t *iter = ll_get_iter_at(list, init_index);
    printf("Iterating forward from index %d\n", init_index);
    ll_node_t *node;
    while (ll_has_next(iter)) {
        node = ll_next(iter);
        print_node_as_int(node);
    }
    printf("\n");
    printf("Iterating backwards\n");
    while (ll_has_previous(iter)) {
        node = ll_previous(iter);
        print_node_as_int(node);
    }
    ll_drop_iter(iter);

    printf("Testing node removal\n");
    iter = ll_get_iter_at(list, init_index);
    if (ll_has_next(iter)) {
        node = ll_next(iter);
        printf("Retrieved node: ");
        print_node_as_int(node);
        printf("Removing...\n");
        ll_remove_last_returned(iter);
        printf("\n");

        print_int_list(list);

        printf("Trying to remove the first node...\n");
        while (ll_has_previous(iter)) {
            node = ll_previous(iter);
        }
        ll_remove_last_returned(iter);

        print_int_list(list);

        printf("Trying to remove the last node...\n");
        while (ll_has_next(iter)) {
            node = ll_next(iter);
        }
        printf("Iteration finished\n");
        print_node_as_int(node->previous);
        ll_remove_last_returned(iter);

        print_int_list(list);

        ll_drop_iter(iter);
    }

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

    printf("Testing iterator...\n");
    test_iter_at(list1, list1->length / 2);

    ll_deinit(list1);
    return EXIT_SUCCESS;
}

