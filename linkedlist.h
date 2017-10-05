/*
 * linkedlist.h
 */

#include <stdlib.h>
#include "node.h"

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

/*
 * Data type for nodes stored in the list.
 * The nodes contain the stored value as a member.
 */
typedef struct ll_node {
    struct ll_node *next;
    struct ll_node *previous;
    void *value;
} ll_node_t;

/*
 * Data type for a linked lists.
 */
typedef struct linkedlist {
    size_t length;
    ll_node_t *head;
    ll_node_t *tail;
    int (*compare_function)(void *value1, void *value2);
} linkedlist_t;

/*
 * Data type for an iterator over a linked list.
 * There is no need to access the members of an iterator directly
 * from code using an iterator; rather, iterator-related functions
 * should be used for all access.
 */
typedef struct ll_iter {
    ll_node_t *next;
    ll_node_t *previous;
    linkedlist_t *list;
    ll_node_t *last_returned;
    char last_skip_forward;
} ll_iter_t;



/*
 * Initializes a new linked list with no nodes and with the default
 * (memory address) comparator function for determining equality of nodes.
 *
 * Returns: a pointer to the initialized list.
 */
linkedlist_t* ll_init();

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
linkedlist_t* ll_init_with_comparator(
    int (*compare_function)(void *value1, void *value2)
);

/*
 * Deinitializes a linked list, freeing the resources used by it.
 * Note that this does not touch the contents the list points to since
 * you may still have pointers to those contents elsewhere.
 * Also, any resources used by iterators must be separately freed
 * by calling ll_drop_iter for each iterator.
 */
void ll_deinit(linkedlist_t *list);

/*
 * Appends a new node with the given value at the end of the list.
 */
void ll_append(linkedlist_t *list, void *value);

/*
 * Inserts a new node with the given value at the given index in the list.
 * If the index is greater than the length of the list, the node is appended
 * at the tail of the list.
 */
void ll_insert(linkedlist_t *list, void *value, size_t index);

/*
 * Returns the index of the first list node containing the given value,
 * or -1 if no such node exists.
 * FIXME: should perhaps return a size_t with an error value indicating non-existence
 */
int ll_index_of(linkedlist_t *list, void *value);

/*
 * Checks whether the given list contains the given value.
 * Returns true (nonzero) if the list contains the value, false (zero) if not.
 */
char ll_contains(linkedlist_t *list, void *value);

/*
 * Removes the first list node containing the given value if such a node exists.
 * Note that this does not free any memory allocated for the stored value itself.
 * Returns: true if removal was successful, false if no matching node was found
 */
char ll_remove(linkedlist_t *list, void *value);

/*
 * Initializes a new iterator for the given list. The new iterator will point
 * in front of the head of the list.
 * 
 * Remember to call ll_drop_iter on the iterator to free the memory used by
 * it when you don't need the iterator anymore.
 *
 * Returns: a pointer to the new iterator
 */
ll_iter_t* ll_get_iter(linkedlist_t *list);

/*
 * Initializes a new iterator for the given list, set to point in front of
 * the node with the given index. If index >= the length of the list,
 * the iterator will point after the last node on the list.
 *
 * Remember to call ll_drop_iter on the iterator to free the memory used by
 * it when you don't need the iterator anymore.
 *
 * Returns: a pointer to the new iterator
 */
ll_iter_t* ll_get_iter_at(linkedlist_t *list, size_t index);

/*
 * Deletes the iterator and frees the memory used by it.
 */
void ll_drop_iter(ll_iter_t *iter);

/*
 * Returns TRUE if the given iterator has a node available for retrieving with
 * a call to ll_iter_next, and FALSE if not.
 */
char ll_iter_has_next(ll_iter_t *iter);

/*
 * Returns TRUE if the given iterator has a node available for retrieving with
 * a call to ll_iter_previous, and FALSE if not.
 */
char ll_iter_has_previous(ll_iter_t *iter);

/*
 * Advances the iterator over the next node on the list.
 * Returns: the node that was passed over, or NULL if none
 */
ll_node_t* ll_iter_next(ll_iter_t *iter);

/*
 * Moves the iterator over the previous node on the list.
 * Returns: the node that was passed over, or NULL if none
 */
ll_node_t* ll_iter_previous(ll_iter_t *iter);

/*
 * Removes the node last returned by the given iterator.
 * Note that this does not free any memory allocated for the stored value itself.
 */
void ll_iter_remove(ll_iter_t *iter);

/*
 * Returns the current length of the given linked list.
 */
size_t ll_length(linkedlist_t *list);

#endif  /* LINKEDLIST_H */
