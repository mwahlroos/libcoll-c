/*
 * ccoll_linkedlist_test.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"
#include "ccoll_linkedlist_test.h"

static int intcmp(void *value1, void *value2) {
    int *a = (int*) value1;
    int *b = (int*) value2;
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

static void print_node_as_int_with_index(ccoll_ll_node_t *node, int index) {
    int *intval = (int*) node->value;
    if (index >= 0) {
        printf("%d ", index);
    }
    printf("(node at 0x%x):\t%d (at 0x%x)\n", (unsigned) node, *intval, (unsigned) intval);
}

static void print_node_as_int(ccoll_ll_node_t *node) {
    print_node_as_int_with_index(node, -1);
}

static void add_int_to_list(ccoll_linkedlist_t *list, int value) {
    int *value_ptr = (int*) malloc (sizeof(int));
    *value_ptr = value;
    ccoll_ll_append(list, value_ptr);
}

static void print_int_list(ccoll_linkedlist_t *list) {
    printf("List length: %d\n", list->length);
    printf("Head: ");
    print_node_as_int(list->head);
    printf("Tail: ");
    print_node_as_int(list->tail);

    ccoll_ll_iter_t *iter = ccoll_ll_get_iter(list);
    ccoll_ll_node_t *node;
    int counter = 0;
    while (ccoll_ll_iter_has_next(iter)) {
        node = ccoll_ll_iter_next(iter);
        print_node_as_int_with_index(node, counter);
        counter++;
    }
    printf("\n");
    ccoll_ll_drop_iter(iter);
}

static void insert_int_at_index(ccoll_linkedlist_t *list, int value, size_t index) {
    int *value_ptr = (int*) malloc (sizeof(int));
    *value_ptr = value;
    ccoll_ll_insert(list, value_ptr, index);
}

static void test_iter_at(ccoll_linkedlist_t *list, size_t init_index) {
    ccoll_ll_iter_t *iter = ccoll_ll_get_iter_at(list, init_index);
    printf("Iterating forward from index %d\n", init_index);
    ccoll_ll_node_t *node;
    while (ccoll_ll_iter_has_next(iter)) {
        node = ccoll_ll_iter_next(iter);
        print_node_as_int(node);
    }
    printf("\n");
    printf("Iterating backwards\n");
    while (ccoll_ll_iter_has_previous(iter)) {
        node = ccoll_ll_iter_previous(iter);
        print_node_as_int(node);
    }
    ccoll_ll_drop_iter(iter);
    printf("\n");

    printf("Testing node removal\n");
    iter = ccoll_ll_get_iter_at(list, init_index);
    if (ccoll_ll_iter_has_next(iter)) {
        node = ccoll_ll_iter_next(iter);
        printf("Retrieved node: ");
        print_node_as_int(node);
        printf("Removing...\n");
        free(node->value);
        ccoll_ll_iter_remove(iter);
        printf("\n");

        print_int_list(list);

        printf("Trying to remove the first node...\n");
        while (ccoll_ll_iter_has_previous(iter)) {
            node = ccoll_ll_iter_previous(iter);
        }
        free(node->value);
        ccoll_ll_iter_remove(iter);

        print_int_list(list);

        printf("Trying to remove the last node...\n");
        while (ccoll_ll_iter_has_next(iter)) {
            node = ccoll_ll_iter_next(iter);
        }
        printf("Iteration finished\n");
        print_node_as_int(node->previous);
        free(node->value);
        ccoll_ll_iter_remove(iter);

        print_int_list(list);

        ccoll_ll_drop_iter(iter);

        printf("Trying to remove all remaining nodes using a new iterator...\n");
        ccoll_ll_iter_t *removal_iter = ccoll_ll_get_iter(list);
        while (ccoll_ll_iter_has_next(removal_iter)) {
            node = ccoll_ll_iter_next(removal_iter);
            print_node_as_int(node);
            free(node->value);
            ccoll_ll_iter_remove(removal_iter);
        }
        printf("Checking that the iterator has no previous node now... ");
        if (ccoll_ll_iter_has_previous(removal_iter)) {
            printf("FAIL\n");
        } else {
            printf("OK\n");
        }
        ccoll_ll_drop_iter(removal_iter);
    }

}

int ccoll_ll_test_main() {
    ccoll_linkedlist_t *list1 = ccoll_ll_init();

    int arrlen = 5;
    int intarr[] = { 2, 3, 5, 7, 11 };
    for (int i=0; i<arrlen; ++i) {
        printf("Trying to insert %d\n", intarr[i]);
        add_int_to_list(list1, intarr[i]);
        print_int_list(list1);
    }
    int insert_test_val = 314;
    size_t insert_test_index = 3U;
    printf("Trying to insert %d at %lu\n", insert_test_val, insert_test_index);
    insert_int_at_index(list1, insert_test_val, insert_test_index);
    print_int_list(list1);

    printf("Testing iterator...\n");
    test_iter_at(list1, list1->length / 2);

    ccoll_ll_deinit(list1);
    return EXIT_SUCCESS;
}

