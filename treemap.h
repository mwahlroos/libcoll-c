/*
 * treemap.h
 *
 * An implementation of a generic key-value map as a binary search tree.
 *
 * Author: Mika Wahlroos 2011
 */

#include <stdlib.h>
#include <stdbool.h>

#ifndef TREEMAP_H
#define TREEMAP_H

#define BT_COLOR_RED   0
#define BT_COLOR_BLACK 1

/* data types */

/* A type for representing single nodes in the tree */
typedef struct treemap_node {
    struct treemap_node *left;
    struct treemap_node *right;
    struct treemap_node *parent;
    void *key;
    void *value;
    char color;
} treemap_node_t;

/* A type for representing a key-value pair.
 * Used for passing stuff around without having to pass actual nodes.
 */
typedef struct treemap_entry {
    void *key;
    void *value;
} treemap_entry_t;

/* A type for representing the tree itself, for holding useful metadata */
typedef struct {
    size_t size;
    treemap_node_t *root;
    int (*key_comparator)(void *key1, void *key2);
} treemap_t;

/* An iterator for iterating through the nodes of a tree in the order of
 * keys as defined by the comparator function (e.g. alphabetical order).
 */
typedef struct {
    treemap_t *tree;
    treemap_node_t *previous;
    treemap_node_t *next;
    treemap_node_t *last_traversed_node;
} treemap_iter_t;


/* external functions */

treemap_t* bt_init();

treemap_t* bt_init_with_comparator(int (*key_comparator_func)(void *key1, void *key2));

void bt_deinit(treemap_t *tree);

void bt_deinit_and_delete_contents(treemap_t *tree);

treemap_node_t* bt_add(treemap_t *tree, void *key, void *value);

treemap_node_t* bt_get(treemap_t *tree, void *key);

bool bt_contains(treemap_t *tree, void *key);

treemap_entry_t bt_remove(treemap_t *tree, void *key);

treemap_node_t* bt_get_successor(treemap_node_t *node);

treemap_node_t* bt_get_predecessor(treemap_node_t *node);

int bt_depth_of(treemap_t *tree, void *key);

treemap_iter_t* bt_get_iterator(treemap_t *tree);

void bt_drop_iterator(treemap_iter_t *iterator);

bool bt_has_next(treemap_iter_t *iterator);

treemap_node_t* bt_next(treemap_iter_t *iterator);

bool bt_has_previous(treemap_iter_t *iterator);

treemap_node_t* bt_previous(treemap_iter_t *iterator);

void bt_remove_last_traversed(treemap_iter_t *iterator);

bool _bt_verify_red_black_conditions(treemap_t *tree);



#endif /* TREEMAP_H */

