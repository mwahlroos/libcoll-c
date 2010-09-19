/*
 * linkedlist.h
 */

#include <stdlib.h>
#include "value.h"

#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#endif

typedef struct ll_node {
    struct ll_node *next;
    struct ll_node *previous;
    value_t *value;
} ll_node_t;

typedef struct ll_iter {
    ll_node_t *next;
    ll_node_t *previous;
    char dirty;
} ll_iter_t;

typedef struct linkedlist {
    size_t length;
    ll_node_t *head;
    ll_node_t *tail;
    ll_iter_t *iter;
} linkedlist_t;

/*
 * Initializes a new linked list.
 * Returns: a pointer to the initialized list.
 */
extern linkedlist_t* ll_init();

/*
 * Deinitializes a linked list, freeing the resources used by it. Note that this
 * does not touch the contents the list points to since you may still have
 * pointers to those contents elsewhere.
 */
extern void ll_deinit(linkedlist_t *list);

/*
 * Appends a new node with the given value at the end of the list.
 */
extern void ll_append(linkedlist_t *list, value_t *value);

/*
 * Returns a pointer to the first list node containing the given value,
 * or NULL if no such node exists.
 */
extern value_t* ll_get(linkedlist_t *list, value_t *value);

/*
 * Removes the first list node containing the given value if such a node exists.
 * Returns: the value contained by the removed node, or NULL if none
 */
extern value_t* ll_remove(linkedlist_t *list, value_t *value);

/*
 * Returns the iterator for the given list. Each list is currently limited to
 * a single iterator, so if one has already been initialized, a pointer to the
 * existing one is given. If not, a new iterator pointing in front of the head
 * of the list is initialized and returned.
 */
extern ll_iter_t* ll_get_iter(linkedlist_t *list);

/*
 * Returns the iterator for the given list, set to point in front of the node
 * with the given index. If index >= the length of the list, the iterator will
 * point after the last node on the list.
 */
extern ll_iter_t* ll_get_iter_at(linkedlist_t *list, size_t index);


extern void ll_drop_iter(ll_iter_t *iter);

/*
 * Advances the iterator over the next node on the list.
 * Returns: the node that was passed over
 */
extern ll_node_t* ll_next(ll_iter_t *iter);

/*
 * Moves the iterator over the previous node on the list.
 * Returns: the node that was passed over
 */
extern ll_node_t* ll_previous(ll_iter_t *iter);

