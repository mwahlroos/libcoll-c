/*
 * linkedlist.c
 */

#include <stdlib.h>
#include "linkedlist.h"

extern linkedlist_t* ll_init()
{
    linkedlist_t *list = (linkedlist_t*) malloc(sizeof(linkedlist_t));
    list->length = 0;
    list->head = list->tail = NULL;
    return list;
}

extern void ll_deinit(linkedlist_t *list)
{
    ll_node_t *node = list->head;
    ll_node_t *next;
    while (node != NULL) {
        next = node->next;
        free(node);
        node = next;
    }
    free(list);
}

extern void ll_append(linkedlist_t *list, value_t *value)
{
    ll_node_t *new_node = (ll_node_t*) malloc(sizeof(ll_node_t));
    new_node->next = NULL;
    new_node->value = value;
    if (list->head == NULL) {
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

extern value_t* ll_get(linkedlist_t *list, value_t *value)
{
    /* FIXME: stub */
    return NULL;
}

extern value_t* ll_remove(linkedlist_t *list, value_t *value)
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
        }
    }
    return iter;
}

extern void ll_drop_iter(ll_iter_t *iter)
{
    free(iter);
}

extern ll_node_t* ll_next(ll_iter_t *iter)
{
    ll_node_t *node = iter->next;
    if (node != NULL) {
        iter->previous = node;
        iter->next = node->next;
    }
    return node;
}

extern ll_node_t* ll_previous(ll_iter_t *iter)
{
    ll_node_t *node = iter->previous;
    if (node != NULL) {
        iter->previous = node->previous;
        iter->next = node;
    }
    return node;
}
