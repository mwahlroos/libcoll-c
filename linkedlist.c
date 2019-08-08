/*
 * linkedlist.c
 */

#include <stdlib.h>
#include <stdio.h>
#include "linkedlist.h"
#include "debug.h"

static void _ccoll_linkedlist_remove_node(ccoll_linkedlist_t *list, ccoll_linkedlist_node_t *node)
{
    if (NULL != node) {
        if (NULL != node->previous) {
            node->previous->next = node->next;
        }
        if (NULL != node->next) {
            node->next->previous = node->previous;
        }
        if (node == list->head) {
            list->head = node->next;
        }
        if (node == list->tail) {
            list->tail = node->previous;
        }
        list->length--;
        free(node);
    }
}

ccoll_linkedlist_t* ccoll_linkedlist_init()
{
    return ccoll_linkedlist_init_with_comparator(NULL);
}

ccoll_linkedlist_t* ccoll_linkedlist_init_with_comparator(int (*compare_function)(void *value1, void *value2))
{
    ccoll_linkedlist_t *list = (ccoll_linkedlist_t*) malloc(sizeof(ccoll_linkedlist_t));
    list->length = 0;
    list->head = list->tail = NULL;
    if (NULL != compare_function) {
        list->compare_function = compare_function;
    } else {
        list->compare_function = &_ccoll_node_comparator_memaddr;
    }
    return list;
}

void ccoll_linkedlist_deinit(ccoll_linkedlist_t *list)
{
    /* remove all nodes first to make sure the memory gets freed */
    ccoll_linkedlist_iter_t *iter = ccoll_linkedlist_get_iter(list);
    while (ccoll_linkedlist_iter_has_next(iter)) {
        ccoll_linkedlist_iter_next(iter);
        ccoll_linkedlist_iter_remove(iter);
    }
    ccoll_linkedlist_drop_iter(iter);

    free(list);
}

void ccoll_linkedlist_append(ccoll_linkedlist_t *list, void *value)
{
    DEBUGF("New node: %d\n", *(int*)(value));
    ccoll_linkedlist_node_t *new_node = (ccoll_linkedlist_node_t*) malloc(sizeof(ccoll_linkedlist_node_t));
    new_node->next = NULL;
    new_node->value = value;
    if (NULL == list->head) {
        DEBUG("Empty list; inserting as first node\n");
        new_node->previous = NULL;
        list->head = list->tail = new_node;
        list->length = 1;
    } else {
        DEBUGF("Inserting after: %d\n", *(int*)(list->tail->value));
        new_node->previous = list->tail;
        list->tail->next = new_node;
        list->tail = new_node;
        list->length++;
    }
    DEBUGF("List head / tail: %d / %d\n", *(int*)(list->head->value),
                                          *(int*)(list->tail->value));
}

void ccoll_linkedlist_insert(ccoll_linkedlist_t *list, void *value, size_t index)
{
    if (index >= list->length) {
        ccoll_linkedlist_append(list, value);
    } else {
        ccoll_linkedlist_node_t *new_node = (ccoll_linkedlist_node_t*) malloc(sizeof(ccoll_linkedlist_node_t));
        new_node->value = value;

        ccoll_linkedlist_node_t *insert_before = list->head;
        size_t counter = 0;
        while (counter < index) {
            insert_before = insert_before->next;
            counter++;
        }
        if (insert_before->previous != NULL) {
            insert_before->previous->next = new_node;
        }
        new_node->previous = insert_before->previous;
        new_node->next = insert_before;

        insert_before->previous = new_node;

        list->length++;
        if (index == 0) {
            list->head = new_node;
        }
    }
}

int ccoll_linkedlist_index_of(ccoll_linkedlist_t *list, void *value)
{
    int retval = -1;
    int counter = 0;

    ccoll_linkedlist_node_t *node = list->head;

    while(NULL != node) {
        if (list->compare_function(value, node->value) == 0) {
            retval = counter;
            break;
        } else {
            node = node->next;
            counter++;
        }
    }

    return retval;
}

char ccoll_linkedlist_contains(ccoll_linkedlist_t *list, void *value)
{
    return ccoll_linkedlist_index_of(list, value) != -1;
}

char ccoll_linkedlist_remove(ccoll_linkedlist_t *list, void *value)
{
    char success = 0;
    ccoll_linkedlist_iter_t *iter = ccoll_linkedlist_get_iter(list);

    while (ccoll_linkedlist_iter_has_next(iter) && !success) {
        void *entry_value = ccoll_linkedlist_iter_next(iter)->value;
        if (list->compare_function(value, entry_value) == 0) {
            ccoll_linkedlist_iter_remove(iter);
            success = 1;
        }
    }

    ccoll_linkedlist_drop_iter(iter);

    return success;
}


ccoll_linkedlist_iter_t* ccoll_linkedlist_get_iter(ccoll_linkedlist_t *list)
{
    ccoll_linkedlist_iter_t *iter = (ccoll_linkedlist_iter_t*) malloc(sizeof(ccoll_linkedlist_iter_t));
    iter->next = list->head;
    iter->previous = NULL;
    iter->list = list;

    return iter;
}

ccoll_linkedlist_iter_t* ccoll_linkedlist_get_iter_at(ccoll_linkedlist_t *list, size_t index)
{
    ccoll_linkedlist_iter_t *iter = ccoll_linkedlist_get_iter(list);
    if (index >= list->length) {
        index = list->length - 1;
    } else {
        size_t counter = 0;
        while (counter < index) {
            ccoll_linkedlist_iter_next(iter);
            counter++;
        }
    }
    return iter;
}

void ccoll_linkedlist_drop_iter(ccoll_linkedlist_iter_t *iter)
{
    free(iter);
}

char ccoll_linkedlist_iter_has_next(ccoll_linkedlist_iter_t *iter)
{
    return (NULL != iter->next);
}

char ccoll_linkedlist_iter_has_previous(ccoll_linkedlist_iter_t *iter)
{
    return (NULL != iter->previous);
}

ccoll_linkedlist_node_t* ccoll_linkedlist_iter_next(ccoll_linkedlist_iter_t *iter)
{
    ccoll_linkedlist_node_t *node = iter->next;
    if (NULL != node) {
        iter->next = node->next;
        iter->previous = node;
    }
    iter->last_returned = node;
    iter->last_skip_forward = 1;
    return node;
}

ccoll_linkedlist_node_t* ccoll_linkedlist_iter_previous(ccoll_linkedlist_iter_t *iter)
{
    ccoll_linkedlist_node_t *node = iter->previous;
    if (NULL != node) {
        iter->previous = node->previous;
        iter->next = node;
    }
    iter->last_returned = node;
    iter->last_skip_forward = 0;
    return node;
}

void ccoll_linkedlist_iter_remove(ccoll_linkedlist_iter_t *iter)
{
    if (NULL != iter->last_returned) {
        if (iter->last_skip_forward) {
            iter->previous = iter->last_returned->previous;
        } else {
            iter->next = iter->last_returned->next;
        }
        _ccoll_linkedlist_remove_node(iter->list, iter->last_returned);
    }
}

size_t ccoll_linkedlist_length(ccoll_linkedlist_t *list)
{
    return list->length;
}

int ccoll_linkedlist_is_empty(ccoll_linkedlist_t *list)
{
    return (list->length == 0);
}
