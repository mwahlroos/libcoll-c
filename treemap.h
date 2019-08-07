/*
 * treemap.h
 *
 * An implementation of a generic key-value map as a binary search tree.
 *
 * Author: Mika Wahlroos 2011
 */

#include <stdlib.h>
#include <stdbool.h>

#ifndef CCOLL_TREEMAP_H
#define CCOLL_TREEMAP_H

/* data types */

/* A type for representing single nodes in the tree */
typedef struct ccoll_treemap_node {
    struct ccoll_treemap_node *left;
    struct ccoll_treemap_node *right;
    struct ccoll_treemap_node *parent;
    void *key;
    void *value;
    char color;
} ccoll_treemap_node_t;

/* A type for representing a key-value pair.
 * Used for passing stuff around without having to pass actual nodes.
 */
typedef struct ccoll_treemap_entry {
    void *key;
    void *value;
} ccoll_treemap_entry_t;

/* A type for representing the tree itself, for holding useful metadata */
typedef struct ccoll_treemap {
    size_t size;
    ccoll_treemap_node_t *root;
    int (*key_comparator)(void *key1, void *key2);
} ccoll_treemap_t;

/* An iterator for iterating through the nodes of a tree in the order of
 * keys as defined by the comparator function (e.g. alphabetical order).
 */
typedef struct ccoll_treemap_iter {
    ccoll_treemap_t *tree;
    ccoll_treemap_node_t *previous;
    ccoll_treemap_node_t *next;
    ccoll_treemap_node_t *last_traversed_node;
} ccoll_treemap_iter_t;


/* external functions */

ccoll_treemap_t* ccoll_treemap_init();

ccoll_treemap_t* ccoll_treemap_init_with_comparator(int (*key_comparator_func)(void *key1, void *key2));

void ccoll_treemap_deinit(ccoll_treemap_t *tree);

void ccoll_treemap_deinit_and_delete_contents(ccoll_treemap_t *tree);

ccoll_treemap_node_t* ccoll_treemap_add(ccoll_treemap_t *tree, void *key, void *value);

ccoll_treemap_node_t* ccoll_treemap_get(ccoll_treemap_t *tree, void *key);

bool ccoll_treemap_contains(ccoll_treemap_t *tree, void *key);

ccoll_treemap_entry_t ccoll_treemap_remove(ccoll_treemap_t *tree, void *key);

ccoll_treemap_node_t* ccoll_treemap_get_successor(ccoll_treemap_node_t *node);

ccoll_treemap_node_t* ccoll_treemap_get_predecessor(ccoll_treemap_node_t *node);

int ccoll_treemap_depth_of(ccoll_treemap_t *tree, void *key);

ccoll_treemap_iter_t* ccoll_treemap_get_iterator(ccoll_treemap_t *tree);

void ccoll_treemap_drop_iterator(ccoll_treemap_iter_t *iterator);

bool ccoll_treemap_has_next(ccoll_treemap_iter_t *iterator);

ccoll_treemap_node_t* ccoll_treemap_next(ccoll_treemap_iter_t *iterator);

bool ccoll_treemap_has_previous(ccoll_treemap_iter_t *iterator);

ccoll_treemap_node_t* ccoll_treemap_previous(ccoll_treemap_iter_t *iterator);

void ccoll_treemap_remove_last_traversed(ccoll_treemap_iter_t *iterator);

bool _ccoll_treemap_verify_red_black_conditions(ccoll_treemap_t *tree);



#endif /* CCOLL_TREEMAP_H */

