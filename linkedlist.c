/*
 * linkedlist.c
 */

#include <stdlib.h>
#include <stdio.h>
#include "linkedlist.h"
#include "debug.h"

static void _ll_remove_node(linkedlist_t *list, ll_node_t *node)
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

linkedlist_t* ll_init()
{
    return ll_init_with_comparator(NULL);
}

linkedlist_t* ll_init_with_comparator(int (*compare_function)(void *value1, void *value2))
{
    linkedlist_t *list = (linkedlist_t*) malloc(sizeof(linkedlist_t));
    list->length = 0;
    list->head = list->tail = NULL;
    if (NULL != compare_function) {
        list->compare_function = compare_function;
    } else {
        list->compare_function = &_node_comparator_memaddr;
    }
    return list;
}

void ll_deinit(linkedlist_t *list)
{
    /* remove all nodes first to make sure the memory gets freed */
    ll_iter_t *iter = ll_get_iter(list);
    while (ll_has_next(iter)) {
        ll_next(iter);
        ll_remove_last_returned(iter);
    }
    ll_drop_iter(iter);

    free(list);
}

void ll_append(linkedlist_t *list, void *value)
{
    DEBUGF("New node: %d\n", *(int*)(value));
    ll_node_t *new_node = (ll_node_t*) malloc(sizeof(ll_node_t));
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

void ll_insert(linkedlist_t *list, void *value, size_t index)
{
    if (index >= list->length) {
        ll_append(list, value);
    } else {
        ll_node_t *new_node = (ll_node_t*) malloc(sizeof(ll_node_t));
        new_node->value = value;
        
        ll_node_t *insert_before = list->head;
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

int ll_index_of(linkedlist_t *list, void *value)
{
    /* FIXME: stub */
    return -1;
}

void* ll_remove(linkedlist_t *list, void *value)
{
    /* FIXME: stub */
    return NULL;
}


ll_iter_t* ll_get_iter(linkedlist_t *list)
{
    ll_iter_t *iter = (ll_iter_t*) malloc(sizeof(ll_iter_t));
    iter->next = list->head;
    iter->previous = NULL;
    iter->list = list;

    return iter;
}

ll_iter_t* ll_get_iter_at(linkedlist_t *list, size_t index)
{
    ll_iter_t *iter = ll_get_iter(list);
    if (index >= list->length) {
        index = list->length - 1;
    } else {
        size_t counter = 0;
        while (counter < index) {
            ll_next(iter);
            counter++;
        }
    }
    return iter;
}

void ll_drop_iter(ll_iter_t *iter)
{
    free(iter);
}

char ll_has_next(ll_iter_t *iter)
{
    return (NULL != iter->next);
}

char ll_has_previous(ll_iter_t *iter)
{
    return (NULL != iter->previous);
}

ll_node_t* ll_next(ll_iter_t *iter)
{
    ll_node_t *node = iter->next;
    if (NULL != node) {
        iter->next = node->next;
        iter->previous = node;
    }
    iter->last_returned = node;
    iter->last_skip_forward = 1;
    return node;
}

ll_node_t* ll_previous(ll_iter_t *iter)
{
    ll_node_t *node = iter->previous;
    if (NULL != node) {
        iter->previous = node->previous;
        iter->next = node;
    }
    iter->last_returned = node;
    iter->last_skip_forward = 0;
    return node;
}

void ll_remove_last_returned(ll_iter_t *iter)
{
    if (NULL != iter->last_returned) {
        if (iter->last_skip_forward) {
            iter->previous = iter->last_returned->previous;
        } else {
            iter->next = iter->last_returned->next;
        }
        _ll_remove_node(iter->list, iter->last_returned);
    }
}

size_t ll_length(linkedlist_t *list)
{
    return list->length;
}
