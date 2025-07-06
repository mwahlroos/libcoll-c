/*
 * linkedlist.c
 *
 * This file is part of libcoll, a generic collections library for C.
 *
 * Copyright (c) 2010-2020 Mika Wahlroos (mika.wahlroos@iki.fi)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>

#include "comparators.h"
#include "linkedlist.h"
#include "list.h"
#include "debug.h"

static void _libcoll_linkedlist_remove_node(libcoll_linkedlist_t *list, libcoll_linkedlist_node_t *node)
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
static void _libcoll_linkedlist_insert_node(libcoll_linkedlist_t *list,
                                            libcoll_linkedlist_node_t *new,
                                            libcoll_linkedlist_node_t *previous,
                                            libcoll_linkedlist_node_t *next) {
    new->previous = previous;
    new->next = next;
    if (NULL != previous) {
        previous->next = new;
    } else {
        list->head = new;
    }
    if (NULL != next) {
        next->previous = new;
    } else {
        list->tail = new;
    }
    list->length++;
}

libcoll_linkedlist_t* libcoll_linkedlist_init()
{
    return libcoll_linkedlist_init_with_comparator(NULL);
}

libcoll_linkedlist_t* libcoll_linkedlist_init_with_comparator(int (*compare_function)(const void *value1, const void *value2))
{
    libcoll_linkedlist_t *list = malloc(sizeof(libcoll_linkedlist_t));
    list->length = 0;
    list->head = list->tail = NULL;
    if (NULL != compare_function) {
        list->compare_function = compare_function;
    } else {
        list->compare_function = &libcoll_memaddrcmp;
    }
    return list;
}

void libcoll_linkedlist_deinit(libcoll_linkedlist_t *list)
{
    /* remove all nodes first to make sure the memory gets freed */
    libcoll_linkedlist_iter_t *iter = libcoll_linkedlist_get_iter(list);
    while (libcoll_linkedlist_iter_has_next(iter)) {
        libcoll_linkedlist_iter_next(iter);
        libcoll_linkedlist_iter_remove(iter);
    }
    libcoll_linkedlist_free_iter(iter);

    free(list);
}

libcoll_list_addition_result_t libcoll_linkedlist_append(libcoll_linkedlist_t *list, void *value)
{
    libcoll_list_addition_result_t result;

    DEBUGF("New node: %d\n", *(int*)(value));
    libcoll_linkedlist_node_t *new_node = malloc(sizeof(libcoll_linkedlist_node_t));
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
    DEBUGF("List head / tail: %p / %p\n", (list->head->value),
                                          (list->tail->value));
    result.status = LIST_ENTRY_ADDED;
    return result;
}

libcoll_list_addition_result_t libcoll_linkedlist_insert(libcoll_linkedlist_t *list, void *value, size_t index)
{
    libcoll_list_addition_result_t result;
    if (index >= list->length) {
        libcoll_linkedlist_append(list, value);
    } else {
        libcoll_linkedlist_node_t *new_node = malloc(sizeof(libcoll_linkedlist_node_t));
        new_node->value = value;

        libcoll_linkedlist_node_t *insert_before = list->head;
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
    result.status = LIST_ENTRY_ADDED;
    return result;
}

libcoll_list_replacement_result_t libcoll_linkedlist_replace_at(libcoll_linkedlist_t *list, void *value, size_t index)
{
    libcoll_list_replacement_result_t result;
    if (index >= list->length) {
        result.status = LIST_REPLACEMENT_FAILED;
        result.error = LIST_INDEX_OUT_OF_RANGE;
        return result;
    }

    libcoll_linkedlist_node_t *node = list->head;
    for (size_t i=0; i<index; i++) {
        node = node->next;
    }

    result.old_value = node->value;
    result.status = LIST_ITEM_REPLACED;

    node->value = value;

    return result;
}

int libcoll_linkedlist_index_of(libcoll_linkedlist_t *list, void *value)
{
    int retval = -1;
    int counter = 0;

    libcoll_linkedlist_node_t *node = list->head;

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

char libcoll_linkedlist_contains(libcoll_linkedlist_t *list, void *value)
{
    return libcoll_linkedlist_index_of(list, value) != -1;
}

char libcoll_linkedlist_remove(libcoll_linkedlist_t *list, void *value)
{
    char success = 0;
    libcoll_linkedlist_iter_t *iter = libcoll_linkedlist_get_iter(list);

    while (libcoll_linkedlist_iter_has_next(iter) && !success) {
        void *entry_value = libcoll_linkedlist_iter_next(iter);
        if (list->compare_function(value, entry_value) == 0) {
            libcoll_linkedlist_iter_remove(iter);
            success = 1;
        }
    }

    libcoll_linkedlist_free_iter(iter);

    return success;
}


libcoll_linkedlist_iter_t* libcoll_linkedlist_get_iter(libcoll_linkedlist_t *list)
{
    libcoll_linkedlist_iter_t *iter = malloc(sizeof(libcoll_linkedlist_iter_t));
    iter->next = list->head;
    iter->previous = NULL;
    iter->list = list;

    return iter;
}

libcoll_linkedlist_iter_t* libcoll_linkedlist_get_iter_at(libcoll_linkedlist_t *list, size_t index)
{
    libcoll_linkedlist_iter_t *iter = libcoll_linkedlist_get_iter(list);
    if (index >= list->length) {
        index = list->length - 1;
    } else {
        size_t counter = 0;
        while (counter < index) {
            libcoll_linkedlist_iter_next(iter);
            counter++;
        }
    }
    return iter;
}

void libcoll_linkedlist_free_iter(libcoll_linkedlist_iter_t *iter)
{
    free(iter);
}

char libcoll_linkedlist_iter_has_next(libcoll_linkedlist_iter_t *iter)
{
    return (NULL != iter->next);
}

char libcoll_linkedlist_iter_has_previous(libcoll_linkedlist_iter_t *iter)
{
    return (NULL != iter->previous);
}

void* libcoll_linkedlist_iter_next(libcoll_linkedlist_iter_t *iter)
{
    libcoll_linkedlist_node_t *node = iter->next;

    iter->last_skip_forward = 1;
    iter->last_returned = node;

    if (NULL != node) {
        iter->next = node->next;
        iter->previous = node;
        return node->value;
    } else {
        return NULL;
    }
}

void* libcoll_linkedlist_iter_previous(libcoll_linkedlist_iter_t *iter)
{
    libcoll_linkedlist_node_t *node = iter->previous;
    if (NULL != node) {
        iter->previous = node->previous;
        iter->next = node;
    }
    iter->last_returned = node;
    iter->last_skip_forward = 0;
    return node->value;
}

void libcoll_linkedlist_iter_insert(libcoll_linkedlist_iter_t *iter, void *value) {
    libcoll_linkedlist_node_t *new_node = malloc(sizeof(libcoll_linkedlist_node_t));
    new_node->value = value;

    new_node->previous = iter->previous;
    new_node->next = iter->next;

    _libcoll_linkedlist_insert_node(iter->list, new_node, iter->previous, iter->next);
    if (iter->last_skip_forward) {
        iter->previous = new_node;
    } else {
        iter->next = new_node;
    }
}

libcoll_list_removal_result_t libcoll_linkedlist_iter_remove(libcoll_linkedlist_iter_t *iter)
{
    libcoll_list_removal_result_t result;
    if (NULL != iter->last_returned) {
        if (iter->last_skip_forward) {
            iter->previous = iter->last_returned->previous;
        } else {
            iter->next = iter->last_returned->next;
        }

        result.value = iter->last_returned->value;

        _libcoll_linkedlist_remove_node(iter->list, iter->last_returned);

        result.status = LIST_ENTRY_REMOVED;
    } else {
        result.status = LIST_REMOVAL_FAILED;
        result.error = LIST_INDEX_OUT_OF_RANGE;
    }

    return result;
}

libcoll_list_replacement_result_t libcoll_linkedlist_iter_replace(libcoll_linkedlist_iter_t *iter, void *value)
{
    libcoll_list_replacement_result_t result;

    libcoll_linkedlist_node_t *node = iter->last_returned;

    if (NULL != node) {
        result.old_value = node->value;
        result.status = LIST_ITEM_REPLACED;
        node->value = value;
    } else {
        /* TODO: should have a more specific error */
        result.error = LIST_INDEX_OUT_OF_RANGE;
        result.status = LIST_REPLACEMENT_FAILED;
    }

    return result;
}

size_t libcoll_linkedlist_length(libcoll_linkedlist_t *list)
{
    return list->length;
}

int libcoll_linkedlist_is_empty(libcoll_linkedlist_t *list)
{
    return (list->length == 0);
}
