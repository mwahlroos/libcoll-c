/*
 * linkedlist.h
 */

#include <stdlib.h>
#include "node.h"

#ifndef CCOLL_LINKEDLIST_H
#define CCOLL_LINKEDLIST_H

/*
 * Data type for nodes stored in the list.
 * The nodes contain the stored value as a member.
 */
typedef struct ccoll_linkedlist_node {
    struct ccoll_linkedlist_node *next;
    struct ccoll_linkedlist_node *previous;
    void *value;
} ccoll_linkedlist_node_t;

/*
 * Data type for a linked lists.
 */
typedef struct ccoll_linkedlist {
    size_t length;
    ccoll_linkedlist_node_t *head;
    ccoll_linkedlist_node_t *tail;
    int (*compare_function)(void *value1, void *value2);
} ccoll_linkedlist_t;

/*
 * Data type for an iterator over a linked list.
 * There is no need to access the members of an iterator directly
 * from code using an iterator; rather, iterator-related functions
 * should be used for all access.
 */
typedef struct ccoll_linkedlist_iter {
    ccoll_linkedlist_node_t *next;
    ccoll_linkedlist_node_t *previous;
    ccoll_linkedlist_t *list;
    ccoll_linkedlist_node_t *last_returned;
    char last_skip_forward;
} ccoll_linkedlist_iter_t;



/*
 * Initializes a new linked list with no nodes and with the default
 * (memory address) comparator function for determining equality of nodes.
 *
 * Returns: a pointer to the initialized list.
 */
ccoll_linkedlist_t* ccoll_linkedlist_init();

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
ccoll_linkedlist_t* ccoll_linkedlist_init_with_comparator(
    int (*compare_function)(void *value1, void *value2)
);

/*
 * Deinitializes a linked list, freeing the resources used by it.
 * Note that this does not touch the contents the list points to since
 * you may still have pointers to those contents elsewhere.
 * Also, any resources used by iterators must be separately freed
 * by calling ccoll_linkedlist_drop_iter for each iterator.
 */
void ccoll_linkedlist_deinit(ccoll_linkedlist_t *list);

/*
 * Appends a new node with the given value at the end of the list.
 */
void ccoll_linkedlist_append(ccoll_linkedlist_t *list, void *value);

/*
 * Inserts a new node with the given value at the given index in the list.
 * If the index is greater than the length of the list, the node is appended
 * at the tail of the list.
 */
void ccoll_linkedlist_insert(ccoll_linkedlist_t *list, void *value, size_t index);

/*
 * Returns the index of the first list node containing the given value,
 * or -1 if no such node exists.
 * FIXME: should perhaps return a size_t with an error value indicating non-existence
 */
int ccoll_linkedlist_index_of(ccoll_linkedlist_t *list, void *value);

/*
 * Checks whether the given list contains the given value.
 * Returns true (nonzero) if the list contains the value, false (zero) if not.
 */
char ccoll_linkedlist_contains(ccoll_linkedlist_t *list, void *value);

/*
 * Removes the first list node containing the given value if such a node exists.
 * Note that this does not free any memory allocated for the stored value itself.
 * Returns: true if removal was successful, false if no matching node was found
 */
char ccoll_linkedlist_remove(ccoll_linkedlist_t *list, void *value);

/*
 * Initializes a new iterator for the given list. The new iterator will point
 * in front of the head of the list.
 *
 * Remember to call ccoll_linkedlist_drop_iter on the iterator to free the memory used by
 * it when you don't need the iterator anymore.
 *
 * Returns: a pointer to the new iterator
 */
ccoll_linkedlist_iter_t* ccoll_linkedlist_get_iter(ccoll_linkedlist_t *list);

/*
 * Initializes a new iterator for the given list, set to point in front of
 * the node with the given index. If index >= the length of the list,
 * the iterator will point after the last node on the list.
 *
 * Remember to call ccoll_linkedlist_drop_iter on the iterator to free the memory used by
 * it when you don't need the iterator anymore.
 *
 * Returns: a pointer to the new iterator
 */
ccoll_linkedlist_iter_t* ccoll_linkedlist_get_iter_at(ccoll_linkedlist_t *list, size_t index);

/*
 * Deletes the iterator and frees the memory used by it.
 */
void ccoll_linkedlist_drop_iter(ccoll_linkedlist_iter_t *iter);

/*
 * Returns TRUE if the given iterator has a node available for retrieving with
 * a call to ccoll_linkedlist_iter_next, and FALSE if not.
 */
char ccoll_linkedlist_iter_has_next(ccoll_linkedlist_iter_t *iter);

/*
 * Returns TRUE if the given iterator has a node available for retrieving with
 * a call to ccoll_linkedlist_iter_previous, and FALSE if not.
 */
char ccoll_linkedlist_iter_has_previous(ccoll_linkedlist_iter_t *iter);

/*
 * Advances the iterator over the next node on the list.
 * Returns: the node that was passed over, or NULL if none
 */
ccoll_linkedlist_node_t* ccoll_linkedlist_iter_next(ccoll_linkedlist_iter_t *iter);

/*
 * Moves the iterator over the previous node on the list.
 * Returns: the node that was passed over, or NULL if none
 */
ccoll_linkedlist_node_t* ccoll_linkedlist_iter_previous(ccoll_linkedlist_iter_t *iter);

/*
 * Removes the node last returned by the given iterator.
 * Note that this does not free any memory allocated for the stored value itself.
 */
void ccoll_linkedlist_iter_remove(ccoll_linkedlist_iter_t *iter);

/*
 * Returns the current length of the given linked list.
 */
size_t ccoll_linkedlist_length(ccoll_linkedlist_t *list);

int ccoll_linkedlist_is_empty(ccoll_linkedlist_t *list);

#endif  /* CCOLL_LINKEDLIST_H */
