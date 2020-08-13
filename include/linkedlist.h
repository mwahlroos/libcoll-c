/*
 * linkedlist.h
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

#include <stdlib.h>

#ifndef LIBCOLL_LINKEDLIST_H
#define LIBCOLL_LINKEDLIST_H

/*
 * Data type for nodes stored in the list.
 * The nodes contain the stored value as a member.
 */
typedef struct libcoll_linkedlist_node {
    struct libcoll_linkedlist_node *next;
    struct libcoll_linkedlist_node *previous;
    void *value;
} libcoll_linkedlist_node_t;

/*
 * Data type for a linked lists.
 */
typedef struct libcoll_linkedlist {
    size_t length;
    libcoll_linkedlist_node_t *head;
    libcoll_linkedlist_node_t *tail;
    int (*compare_function)(const void *value1, const void *value2);
} libcoll_linkedlist_t;

/*
 * Data type for an iterator over a linked list.
 * There is no need to access the members of an iterator directly
 * from code using an iterator; rather, iterator-related functions
 * should be used for all access.
 */
typedef struct libcoll_linkedlist_iter {
    libcoll_linkedlist_node_t *next;
    libcoll_linkedlist_node_t *previous;
    libcoll_linkedlist_t *list;
    libcoll_linkedlist_node_t *last_returned;
    char last_skip_forward;
} libcoll_linkedlist_iter_t;



/*
 * Initializes a new linked list with no nodes and with the default
 * (memory address) comparator function for determining equality of nodes.
 *
 * Returns: a pointer to the initialized list.
 */
libcoll_linkedlist_t* libcoll_linkedlist_init();

/*
 * Initializes a new linked list with no nodes.
 *
 * The given compare function will be used for determining the equality of node
 * values when finding nodes by value. The function must return 0 for equal and
 * non-zero for nonequal values. If the argument is NULL, the equality will be
 * based on comparing memory addresses.
 *
 * Returns: a pointer to the initialized list.
 */
libcoll_linkedlist_t* libcoll_linkedlist_init_with_comparator(
    int (*compare_function)(const void *value1, const void *value2)
);

/*
 * Deinitializes a linked list, freeing the resources used by it.
 * Note that this does not touch the contents the list points to since
 * you may still have pointers to those contents elsewhere.
 * Also, any resources used by iterators must be separately freed
 * by calling libcoll_linkedlist_drop_iter for each iterator.
 */
void libcoll_linkedlist_deinit(libcoll_linkedlist_t *list);

/*
 * Appends a new node with the given value at the end of the list.
 */
void libcoll_linkedlist_append(libcoll_linkedlist_t *list, void *value);

/*
 * Inserts a new node with the given value at the given index in the list.
 * If the index is greater than the length of the list, the node is appended
 * at the tail of the list.
 */
void libcoll_linkedlist_insert(libcoll_linkedlist_t *list, void *value, size_t index);

/*
 * Returns the index of the first list node containing the given value,
 * or -1 if no such node exists.
 * FIXME: should perhaps return a size_t with an error value indicating non-existence
 */
int libcoll_linkedlist_index_of(libcoll_linkedlist_t *list, void *value);

/*
 * Checks whether the given list contains the given value.
 * Returns true (nonzero) if the list contains the value, false (zero) if not.
 */
char libcoll_linkedlist_contains(libcoll_linkedlist_t *list, void *value);

/*
 * Removes the first list node containing the given value if such a node exists.
 * Note that this does not free any memory allocated for the stored value itself.
 * Returns: true if removal was successful, false if no matching node was found
 */
char libcoll_linkedlist_remove(libcoll_linkedlist_t *list, void *value);

/*
 * Initializes a new iterator for the given list. The new iterator will point
 * in front of the head of the list.
 *
 * Remember to call libcoll_linkedlist_drop_iter on the iterator to free the memory used by
 * it when you don't need the iterator anymore.
 *
 * Returns: a pointer to the new iterator
 */
libcoll_linkedlist_iter_t* libcoll_linkedlist_get_iter(libcoll_linkedlist_t *list);

/*
 * Initializes a new iterator for the given list, set to point in front of
 * the node with the given index. If index >= the length of the list,
 * the iterator will point after the last node on the list.
 *
 * Remember to call libcoll_linkedlist_drop_iter on the iterator to free the memory used by
 * it when you don't need the iterator anymore.
 *
 * Returns: a pointer to the new iterator
 */
libcoll_linkedlist_iter_t* libcoll_linkedlist_get_iter_at(libcoll_linkedlist_t *list, size_t index);

/*
 * Deletes the iterator and frees the memory used by it.
 */
void libcoll_linkedlist_drop_iter(libcoll_linkedlist_iter_t *iter);

/*
 * Returns TRUE if the given iterator has a node available for retrieving with
 * a call to libcoll_linkedlist_iter_next, and FALSE if not.
 */
char libcoll_linkedlist_iter_has_next(libcoll_linkedlist_iter_t *iter);

/*
 * Returns TRUE if the given iterator has a node available for retrieving with
 * a call to libcoll_linkedlist_iter_previous, and FALSE if not.
 */
char libcoll_linkedlist_iter_has_previous(libcoll_linkedlist_iter_t *iter);

/*
 * Advances the iterator over the next node on the list.
 * Returns: the value at the node that was passed over, or NULL if none
 */
void* libcoll_linkedlist_iter_next(libcoll_linkedlist_iter_t *iter);

/*
 * Moves the iterator over the previous node on the list.
 * Returns: the value at the node that was passed over, or NULL if none
 */
void* libcoll_linkedlist_iter_previous(libcoll_linkedlist_iter_t *iter);

/*
 * Removes the node last returned by the given iterator.
 * Note that this does not free any memory allocated for the stored value itself.
 */
void libcoll_linkedlist_iter_remove(libcoll_linkedlist_iter_t *iter);

/*
 * Returns the current length of the given linked list.
 */
size_t libcoll_linkedlist_length(libcoll_linkedlist_t *list);

int libcoll_linkedlist_is_empty(libcoll_linkedlist_t *list);

#endif  /* LIBCOLL_LINKEDLIST_H */
