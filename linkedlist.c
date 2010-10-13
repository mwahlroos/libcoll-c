/*
 * linkedlist.c
 */

#include <stdlib.h>
#include "linkedlist.h"

extern linkedlist_t* ll_init(int (*compare_function)(node_value_t *v1, node_value_t *v2))
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

extern void ll_deinit(linkedlist_t *list)
{
    ll_node_t *node = list->head;
    ll_node_t *next;
    while (NULL != node) {
        next = node->next;
        free(node);
        node = next;
    }
    free(list);
}

extern void ll_append(linkedlist_t *list, node_value_t *value)
{
    ll_node_t *new_node = (ll_node_t*) malloc(sizeof(ll_node_t));
    new_node->next = NULL;
    new_node->value = value;
    if (NULL == list->head) {
        new_node->previous = NULL;
        list->head = list->tail = new_node;
        list->length = 1;
    } else {
        new_node->previous = list->tail;
        list->tail->next = new_node;
        list->tail = new_node;
        list->length++;
    }
}

extern void ll_insert(linkedlist_t *list, node_value_t *value, size_t index)
{
    if (index >= list->length) {
        ll_append(list, value);
    } else {
        ll_node_t *new_node = (ll_node_t*) malloc(sizeof(ll_node_t));
        
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

extern node_value_t* ll_get(linkedlist_t *list, node_value_t *value)
{
    /* FIXME: stub */
    return NULL;
}

extern node_value_t* ll_remove(linkedlist_t *list, node_value_t *value)
{
    /* FIXME: stub */
    return NULL;
}


extern ll_iter_t* ll_get_iter(linkedlist_t *list)
{
    ll_iter_t *iter = (ll_iter_t*) malloc(sizeof(ll_iter_t));
    iter->next = list->head;
    iter->previous = NULL;

    return iter;
}

extern ll_iter_t* ll_get_iter_at(linkedlist_t *list, size_t index)
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

extern void ll_drop_iter(ll_iter_t *iter)
{
    free(iter);
}

extern char ll_has_next(ll_iter_t *iter)
{
    return (NULL != iter->next);
}

extern char ll_has_previous(ll_iter_t *iter)
{
    return (NULL != iter->previous);
}

extern ll_node_t* ll_next(ll_iter_t *iter)
{
    ll_node_t *node = iter->next;
    if (NULL != node) {
        iter->previous = node;
        iter->next = node->next;
    }
    return node;
}

extern ll_node_t* ll_previous(ll_iter_t *iter)
{
    ll_node_t *node = iter->previous;
    if (NULL != node) {
        iter->previous = node->previous;
        iter->next = node;
    }
    return node;
}
