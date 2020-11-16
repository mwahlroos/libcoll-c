/*
 * treemap.c
 *
 * This file is part of libcoll, a generic collections library for C.
 *
 * An implementation of a generic key-value map as a binary search tree.
 *
 * The tree provides a data structure for mapping between keys and values;
 * in other words, values are accessed through associated keys.
 *
 * Any pointer data type can be used for keys and values.  However, there is
 * no built-in type safety, so objects retrieved from the tree must be
 * explicitly cast back to the correct pointer type before dereferencing.
 * Duplicate keys are not stored.
 *
 * A comparator function for determining the equality and mutual order of keys
 * may be provided when initializing a new tree.  If no comparator function is
 * explicitly specified, keys will be compared by memory address.
 *
 * This implementation of the binary tree API is based on a self-balancing
 * red-black tree.  Most of the algorithms are adaptations from the book
 * Introduction to Algorithms, 2nd ed., by Cormen, Leiserson, Rivest and Stein.
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
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. *
 */

#include <stdbool.h>
#include <stdlib.h>

#include "comparators.h"
#include "treemap.h"

#include "debug.h"

#define COLOR_RED   0
#define COLOR_BLACK 1

/* define a null node for use as black leaf nodes in the red-black tree */
static libcoll_treemap_node_t null_node_struct = {
    NULL, NULL, NULL, NULL, NULL, COLOR_BLACK
};
static libcoll_treemap_node_t *NULL_NODE = &null_node_struct;

/* declarations of static helper functions for internal use */
static libcoll_treemap_node_t* create_node(void *key, void *value);
static void remove_node(libcoll_treemap_t *tree, libcoll_treemap_node_t *node);
static void deinit_subtree(libcoll_treemap_node_t *node, bool free_contents);
static void left_rotate(libcoll_treemap_t *tree, libcoll_treemap_node_t *subtree_orig_root);
static void right_rotate(libcoll_treemap_t *tree, libcoll_treemap_node_t *subtree_orig_root);
static void fix_after_addition(libcoll_treemap_t *tree, libcoll_treemap_node_t *added_node);
static void fix_after_removal(libcoll_treemap_t *tree, libcoll_treemap_node_t *removed_node);

/* declarations of helpers used for testing */
static bool _verify_child_color_in_subtree(libcoll_treemap_node_t *subtree_root);
static int  _verify_black_height_of_subtree(libcoll_treemap_node_t *subtree_root);


/* external API functions */

/*
 * Initializes a new binary tree.
 * The new tree will use the default comparator (comparison by memory address)
 * for determining the (in)equality and mutual order of keys.
 *
 * If allocating memory for the tree fails, NULL is returned.
 *
 * Returns: a pointer to the newly allocated tree
 */
libcoll_treemap_t* libcoll_treemap_init()
{
    return libcoll_treemap_init_with_comparator(NULL);
}

/*
 * Initializes a new binary tree with the given key comparator function.
 *
 * If allocating memory for the tree fails, NULL is returned.
 *
 * Returns: a pointer to the newly allocated tree
 */
libcoll_treemap_t* libcoll_treemap_init_with_comparator (int (*key_comparator)(const void *key1, const void *key2))
{
    DEBUG("treemap initializing\n");
    libcoll_treemap_t *tree = malloc(sizeof(libcoll_treemap_t));
    if (NULL != tree) {
        tree->root = NULL_NODE;
        tree->size = 0;
        if (NULL != key_comparator) {
            tree->key_comparator = key_comparator;
        } else {
            // fall back to the default behaviour of comparison by memory address
            tree->key_comparator = &libcoll_memaddrcmp;
        }
    }
    return tree;
}

/*
 * Deinitializes a binary tree, freeing the memory allocated for the tree and
 * its nodes.
 *
 * This does not free the memory allocated for any of the actual keys and
 * values stored in the nodes since those values may still be in use
 * somewhere else.  The actual stored objects will need to be freed
 * separately when they are no longer needed.
 *
 * Params:
 *      tree -- the binary tree to be deinitialized
 */
void libcoll_treemap_deinit(libcoll_treemap_t *tree)
{
    /* deallocate all nodes first to make sure their memory gets freed */
    deinit_subtree(tree->root, false);
    free(tree);
}

/*
 * Deinitializes a binary tree, freeing the memory allocated for the tree, its
 * nodes, and any keys and values stored in the nodes.
 *
 * This may be useful if there is no need for any of the stored data anymore or
 * if all relevant objects have already been copied elsewhere.
 *
 * Params:
 *      tree -- the binary tree to be deinitialized
 */
void libcoll_treemap_deinit_and_delete_contents(libcoll_treemap_t *tree)
{
    deinit_subtree(tree->root, true);
    free(tree);
}

/*
 * Adds a new node with the given key and value into the tree.
 * No duplicate keys will be stored.  If a node with a key equal to the
 * given one already exists, no new node will be created, and NULL is returned.
 *
 * Params:
 *      tree  -- the tree to which the value should be added
 *      key   -- the key to be added
 *      value -- the value to be added
 *
 * Returns: a pointer to the newly created node, or NULL if adding the node
 *          failed due to an already existing key or due to malloc failing
 */
libcoll_treemap_node_t* libcoll_treemap_add(libcoll_treemap_t *tree, void *key, void *value)
{
    DEBUG("Adding new key\n");
    libcoll_treemap_node_t *new_node;

    libcoll_treemap_node_t *parent = tree->root;
    if (NULL_NODE == parent) {
        new_node = create_node(key, value);
        if (NULL == new_node)  return NULL;

        tree->root = new_node;
    } else {
        bool place_found = false;
        while (!place_found) {
            int cmpval = tree->key_comparator(key, parent->key);
            if (cmpval < 0) {
                if (NULL_NODE != parent->left) {
                    parent = parent->left;
                } else {
                    new_node = create_node(key, value);
                    if (NULL == new_node)  return NULL;

                    parent->left = new_node;
                    new_node->parent = parent;
                    place_found = true;
                }
            } else if (cmpval > 0) {
                if (NULL_NODE != parent->right) {
                    parent = parent->right;
                } else {
                    new_node = create_node(key, value);
                    if (NULL == new_node)  return NULL;

                    parent->right = new_node;
                    new_node->parent = parent;
                    place_found = true;
                }
            } else {
                DEBUGF("Found existing node (at %p) with equal key\n", (void*) parent);
                new_node = NULL;
                place_found = true;
            }
        }
    }
    DEBUG("\n");
    if (NULL != new_node) {
        fix_after_addition(tree, new_node);
        tree->size++;
    }

    return new_node;
}

/*
 * Gets the node with the specified key in the given tree.
 * If no such node can be found in the tree, NULL is returned.
 *
 * Params:
 *      tree -- the tree in which to search
 *      key  -- the key for which to search
 *
 * Returns: a pointer to the node with the given key, or NULL if none
 */
libcoll_treemap_node_t* libcoll_treemap_get(libcoll_treemap_t *tree, void *key)
{
    libcoll_treemap_node_t *node = tree->root;
    while (NULL_NODE != node) {
        int cmpval = tree->key_comparator(key, node->key);
        if (cmpval < 0) {
            node = node->left;
        } else if (cmpval > 0) {
            node = node->right;
        } else {
            break;
        }
    }

    /* let's not expose the internal null node business in the external API,
     * so return an ordinary NULL pointer instead
     */
    if (NULL_NODE == node) {
        node = NULL;
    }
    return node;
}

/*
 * Checks whether the tree already contains a node with the given key.
 *
 * Params:
 *      tree -- the three in which to check for the key
 *      key  -- the key to check
 *
 * Returns: true if a node with the given key exists, false if not
 */
bool libcoll_treemap_contains(libcoll_treemap_t *tree, void *key)
{
    return (NULL != libcoll_treemap_get(tree, key));
}

/*
 * Removes the node with the given key from the tree.
 * If no node with such a key exists in the tree, nothing happens.
 *
 * Params:
 *      tree -- the tree from which to remove the node
 *      key  -- the key of the node to remove
 *
 * Returns: a struct that contains the original key and value of the removed node.
 *          The key in the struct will be set to NULL if no node with a
 *          matching key was found in the tree.
 */
libcoll_pair_voidptr_t libcoll_treemap_remove(libcoll_treemap_t *tree, void *key)
{
    libcoll_pair_voidptr_t pair;
    libcoll_treemap_node_t *node = libcoll_treemap_get(tree, key);
    if (NULL_NODE != node) {
        pair.a = node->key;
        pair.b = node->value;
        remove_node(tree, node);
    } else {
        pair.a = NULL;
        pair.b = NULL;
    }
    return pair;
}

/*
 * Finds the successor of the given node in a tree.
 * The successor is the node with the next larger key in the tree
 * in the sorting order determined by the comparator function.
 *
 * Params:
 *      node -- the node whose successor is being sought
 *
 * Returns: the successor of the node, or NULL if the node has no successor
 */
libcoll_treemap_node_t* libcoll_treemap_get_successor(libcoll_treemap_node_t *node)
{
    // algorithm adapted from CLRS
    DEBUGF("Finding successor for node @ %p\n", (void*) node);
    libcoll_treemap_node_t *candidate;
    if (NULL_NODE != node->right) {
        candidate = node->right;
        while (NULL_NODE != candidate->left) {
            candidate = candidate->left;
        }
    } else {
        libcoll_treemap_node_t *parent = node->parent;
        candidate = node;
        while (NULL_NODE != parent && candidate == parent->right) {
            candidate = parent;
            parent = parent->parent;
        }
        candidate = parent;
    }
    return candidate;
}

/*
 * Finds the predecessor of the given node in the tree.
 * This function is symmetrical to libcoll_treemap_get_successor.
 *
 * Params:
 *      node -- the node whose predecessor is being sought
 *
 * Returns: the predecessor of the node, or NULL if the node has no predecessor
 */
libcoll_treemap_node_t* libcoll_treemap_get_predecessor(libcoll_treemap_node_t *node)
{
    // algorithm adapted from CLRS
    DEBUGF("Finding predecessor for node @ %p\n", (void*) node);
    libcoll_treemap_node_t *candidate;
    if (NULL_NODE != node->left) {
        candidate = node->left;
        while (NULL_NODE != candidate->right) {
            candidate = candidate->right;
        }
    } else {
        libcoll_treemap_node_t *parent = node->parent;
        candidate = node;
        while (NULL_NODE != parent && candidate == parent->left) {
            candidate = parent;
            parent = parent->parent;
        }
        candidate = parent;
    }
    return candidate;
}

/*
 * Finds the depth of the node that has the given key in the given tree.
 * If there is no such node in the tree, -1 is returned.
 *
 * Params:
 *      key -- the key of the node whose depth is to be returned
 *
 * Returns: the depth of the node
 */
int libcoll_treemap_depth_of(libcoll_treemap_t *tree, void *key)
{
    libcoll_treemap_node_t *node = tree->root;
    int depth = 0;
    bool found = false;
    while (NULL_NODE != node && !found) {
        int cmpval = tree->key_comparator(key, node->key);
        if (cmpval == 0) {
            found = true;
        } else if (cmpval < 0) {
            node = node->left;
            depth++;
        } else {
            node = node->right;
            depth++;
        }
    }
    if (!found) {
        depth = -1;
    }
    return depth;
}

/*
 * Returns the current number of elements in the treemap.
 */
size_t libcoll_treemap_get_size(libcoll_treemap_t *tree)
{
    return tree->size;
}

/*
 * Returns true if the map currently has zero elements, false otherwise.
 */
char libcoll_treemap_is_empty(libcoll_treemap_t *tree)
{
    return tree->size == 0;
}

/*
 * Initializes a new iterator for the binary tree, set to point in front of the
 * node with the minimum key in the tree as defined by the comparator function.
 *
 * If the structure of the tree is modified through other means while iterating,
 * anything can happen.  Since the iterator does not guard against or notice
 * external modification, care must be taken not to modify the tree during
 * iteration.  It is safe to modify the tree through the iterator itself,
 * though, for example by calling tm_remove_last_traversed.
 *
 * The tm_drop_iter function must be called when the iterator is no longer used
 * in order to free the memory allocated for the iterator.
 *
 * If allocating memory for the iterator fails, NULL is returned.
 *
 * Params:
 *      tree -- the tree the iterator should traverse
 *
 * Returns: a pointer to the new iterator
 */
libcoll_treemap_iter_t* libcoll_treemap_get_iterator(libcoll_treemap_t *tree)
{
    libcoll_treemap_iter_t *iter = malloc (sizeof(libcoll_treemap_iter_t));
    if (NULL != iter) {
        iter->tree = tree;
        libcoll_treemap_node_t *first = tree->root;
        if (NULL_NODE != first) {
            while (NULL_NODE != first->left) {
                first = first->left;
            }
        }
        iter->next = first;
    }
    return iter;
}

/*
 * Deletes the given iterator and frees the memory allocated for it.
 *
 * Params:
 *      iterator -- the iterator to deinitialize
 */
void libcoll_treemap_drop_iterator(libcoll_treemap_iter_t *iterator)
{
    free(iterator);
}

/*
 * Checks whether the given iterator has more nodes left that can be retrieved
 * by calling tm_next.
 *
 * Params:
 *      iterator -- the iterator to check
 *
 * Returns: true if there are more nodes to iterate over, false if not
 */
bool libcoll_treemap_has_next(libcoll_treemap_iter_t *iterator)
{
    return (NULL_NODE != iterator->next);
}

/*
 * Retrieves the next element in the tree in the order of keys.
 *
 * Params:
 *      iterator -- the iterator whose next element to retrieve
 *
 * Returns: the next node in the tree
 */
libcoll_treemap_node_t* libcoll_treemap_next(libcoll_treemap_iter_t *iterator)
{
    libcoll_treemap_node_t *traversed_node = iterator->next;
    iterator->previous = traversed_node;
    iterator->next = libcoll_treemap_get_successor(traversed_node);
    iterator->last_traversed_node = traversed_node;

    return traversed_node;
}

/*
 * Checks whether the given iterator has mode nodes left that can be retrieved
 * by calling tm_previous (i.e. iterating in reverse order of keys).
 *
 * Params:
 *      iterator -- the iterator to check
 *
 * Returns: true of there are more nodes to iterate over, false if not
 */
bool libcoll_treemap_has_previous(libcoll_treemap_iter_t *iterator)
{
    return (NULL_NODE != iterator->previous);
}

/*
 * Retrieves the previous element in the tree in the reverse order of keys.
 *
 * Params:
 *      iterator -- the iterator whose previous element to retrieve
 *
 * Returns: the previous node in the tree
 */
libcoll_treemap_node_t* libcoll_treemap_previous(libcoll_treemap_iter_t *iterator)
{
    libcoll_treemap_node_t *traversed_node = iterator->previous;
    iterator->next = traversed_node;
    iterator->previous = libcoll_treemap_get_predecessor(traversed_node);
    iterator->last_traversed_node = traversed_node;

    return traversed_node;
}

/*
 * Removes the node last traversed and returned by the given iterator from
 * the tree and frees the memory allocated for it.
 *
 * This does not deallocate the stored key and value themselves since
 * there may still be references to them elsewhere outside of the tree.
 * The actual objects stored must be separately freed when they are
 * no longer needed.
 *
 * Params:
 *      iterator -- the iterator whose last traversed node is to be removed
 *
 * Returns a struct that contains a pointer to the key as the first member
 * and a pointer to the value as the second member.
 */
libcoll_pair_voidptr_t libcoll_treemap_remove_last_traversed(libcoll_treemap_iter_t *iterator)
{
    libcoll_pair_voidptr_t pair;
    if (NULL_NODE != iterator->last_traversed_node) {
        libcoll_treemap_node_t *to_be_removed = iterator->last_traversed_node;
        pair.a = to_be_removed->key;
        pair.b = to_be_removed->value;

        if (iterator->last_traversed_node == iterator->previous) {
            iterator->previous = libcoll_treemap_get_predecessor(iterator->previous);
            iterator->last_traversed_node = NULL_NODE;
        } else {
            iterator->next = libcoll_treemap_get_successor(iterator->next);
            iterator->last_traversed_node = NULL_NODE;
        }
        remove_node(iterator->tree, to_be_removed);
    } else {
        pair.a = NULL;
        pair.b = NULL;
    }
    return pair;
}

/*
 * Traverses the given tree and checks whether it is a valid red-black tree.
 * More specifically, this function checks that the following conditions hold
 * in the tree:
 *
 * 1. The root node is black
 * 2. If a node is red, both of its children must be black
 * 3. All paths from a given node down to descendant leaves contain the same
 *    number of black nodes, i.e. the black-height of every node is well-defined.
 *
 * These conditions are as given in CLRS, excluding the two conditions for
 * every node having a colour and for leaf nodes being black, which are
 * implicitly satisfied in this implementation.
 *
 * Returns true if the tree is a valid r-b tree and false if not.
 *
 * This function is for testing purposes.
 */
bool _libcoll_treemap_verify_red_black_conditions(libcoll_treemap_t *tree)
{
    bool tree_valid = true;
    if (NULL_NODE != tree->root && COLOR_RED == tree->root->color) {
        DEBUGF("Tree @ %p is not a valid R-B tree: root node is red\n", (void*) tree);
        tree_valid = false;
    }
    if (! _verify_child_color_in_subtree(tree->root)) {
        DEBUGF("Child colour condition failed for tree @ %p\n", (void*) tree);
        tree_valid = false;
    }
    int black_height = _verify_black_height_of_subtree(tree->root);
    if (-1 == black_height) {
        DEBUGF("Black-height condition failed for tree @ %p\n", (void*) tree);
        tree_valid = false;
    }

    return tree_valid;
}



/* functions for internal use */

/*
 * Allocates and initializes a new node in the tree.
 */
static libcoll_treemap_node_t* create_node(void *key, void *value)
{
    libcoll_treemap_node_t *new_node = malloc(sizeof(libcoll_treemap_node_t));
    if (NULL != new_node) {
        new_node->key = key;
        new_node->value = value;
        new_node->left = new_node->right = new_node->parent = NULL_NODE;
        new_node->color = COLOR_RED;
    }
    return new_node;
}

/*
 * Removes the given node, frees the memory used by it, and rebalances the tree.
 * If the node does not exist in the tree, the results are undefined, so this
 * function should only be called on nodes that are guaranteed to exist.
 *
 * Algorithm adapted from CLRS.
 */
static void remove_node(libcoll_treemap_t *tree, libcoll_treemap_node_t *node)
{
    libcoll_treemap_node_t *spliced_out_node;
    libcoll_treemap_node_t *replacement_node;

    DEBUGF("Got request to remove node @ %p\n", (void*) node);

    if (NULL_NODE == node->left || NULL_NODE == node->right) {
        spliced_out_node = node;
    } else {
        spliced_out_node = libcoll_treemap_get_successor(node);
    }

    DEBUGF("Actual node to splice out from the tree is @ %p\n",
           (void*) spliced_out_node);

    /* due to definition of successor, the node to be spliced out has at most
     * one child at this point
     */
    if (NULL_NODE != spliced_out_node->left) {
        replacement_node = spliced_out_node->left;
    } else {
        replacement_node = spliced_out_node->right;
    }

    /* this might set a parent to a sentinel (null) node, but this helps
     * the red-black fixup function in that special case and has no negative
     * side effects since the parent of the null node is never dereferenced
     * anywhere except in fixup after removal.
     * (part of the algorithm from CLRS)
     */
    replacement_node->parent = spliced_out_node->parent;

    if (NULL_NODE == spliced_out_node->parent) {
        tree->root = replacement_node;
    } else {
        libcoll_treemap_node_t *parent = spliced_out_node->parent;
        if (spliced_out_node == parent->left) {
            parent->left = replacement_node;
        } else {
            parent->right = replacement_node;
        }
    }

    if (spliced_out_node != node) {
        /* actually spliced out the successor of node rather than node itself,
         * so copy the payload of the actually spliced out node
         * on top of those of the node that was supposed to be removed
         */
        node->key = spliced_out_node->key;
        node->value = spliced_out_node->value;
    }

    if (COLOR_BLACK == spliced_out_node->color) {
        fix_after_removal(tree, replacement_node);
    }
    free(spliced_out_node);
    tree->size--;
}

/*
 * Recursively removes all nodes in the subtree rooted at the given node.
 * If free_contents is true, keys and values are also freed.
 * Used for bulk removal of nodes when deinitializing a tree.
 */
static void deinit_subtree(libcoll_treemap_node_t *node, bool free_contents)
{
    if (NULL_NODE != node) {
        deinit_subtree(node->left, free_contents);
        deinit_subtree(node->right, free_contents);
        if (free_contents) {
            free(node->key);
            free(node->value);
        }
        free(node);
    }
}

/*
 * Performs a left rotation of the subtree rooted at the given node.
 * Assumes that the right child of the given node is not the null node.
 * This function directly modifies the structure of the tree.
 *
 * Algorithm adapted from CLRS.
 */
static void left_rotate(libcoll_treemap_t *tree, libcoll_treemap_node_t *subtree_orig_root)
{
    libcoll_treemap_node_t *pivot = subtree_orig_root->right;
    subtree_orig_root->right = pivot->left;

    if (NULL_NODE != subtree_orig_root->right) {
        subtree_orig_root->right->parent = subtree_orig_root;
    }

    libcoll_treemap_node_t *parent_of_subtree = subtree_orig_root->parent;
    pivot->parent = parent_of_subtree;
    if (NULL_NODE == parent_of_subtree) {
        tree->root = pivot;
    } else {
        if (subtree_orig_root == parent_of_subtree->left) {
            parent_of_subtree->left = pivot;
        } else {
            parent_of_subtree->right = pivot;
        }
    }
    pivot->left = subtree_orig_root;
    subtree_orig_root->parent = pivot;
}

/*
 * Performs a right rotation of the subtree rooted at the given node.
 * This function is symmetric to left_rotate.
 *
 * Algorithm adapted from CLRS.
 */
static void right_rotate(libcoll_treemap_t *tree, libcoll_treemap_node_t *subtree_orig_root)
{
    libcoll_treemap_node_t *pivot = subtree_orig_root->left;
    subtree_orig_root->left = pivot->right;

    if (NULL_NODE != subtree_orig_root->left) {
        subtree_orig_root->left->parent = subtree_orig_root;
    }

    libcoll_treemap_node_t *parent_of_subtree = subtree_orig_root->parent;
    pivot->parent = parent_of_subtree;
    if (NULL_NODE == parent_of_subtree) {
        tree->root = pivot;
    } else {
        if (subtree_orig_root == parent_of_subtree->right) {
            parent_of_subtree->right = pivot;
        } else {
            parent_of_subtree->left = pivot;
        }
    }
    pivot->right = subtree_orig_root;
    subtree_orig_root->parent = pivot;
}

/*
 * Ensures that the red-black conditions continue to hold after the the given
 * node has been added into the tree.
 *
 * Algorithm adapted from CLRS.
 */
static void fix_after_addition(libcoll_treemap_t *tree, libcoll_treemap_node_t *node_added)
{
    libcoll_treemap_node_t *node = node_added;
    libcoll_treemap_node_t *parent;
    libcoll_treemap_node_t *uncle;
    while (COLOR_RED == node->parent->color) {
        // parent can't be the null node at this point since it's red
        parent = node->parent;
        if (parent == parent->parent->left) {
            uncle = parent->parent->right;
            if (COLOR_RED == uncle->color) {
                // CLRS case 1, uncle is red
                parent->color = COLOR_BLACK;
                uncle->color = COLOR_BLACK;
                parent->parent->color = COLOR_RED;
                node = parent->parent;
            } else {
                if (node == parent->right) {
                    // CLRS case 2
                    node = parent;
                    left_rotate(tree, node);
                }
                // CLRS case 3
                node->parent->color = COLOR_BLACK;
                node->parent->parent->color = COLOR_RED;
                right_rotate(tree, node->parent->parent);
            }
        } else {
            // symmetric to the if (...) branch
            uncle = parent->parent->left;
            if (COLOR_RED == uncle->color) {
                // case 1
                parent->color = COLOR_BLACK;
                uncle->color = COLOR_BLACK;
                parent->parent->color = COLOR_RED;
                node = parent->parent;
            } else {
                if (node == parent->left) {
                    // case 2
                    node = parent;
                    right_rotate(tree, node);
                }
                // case 3
                node->parent->color = COLOR_BLACK;
                node->parent->parent->color = COLOR_RED;
                left_rotate(tree, node->parent->parent);
            }
        }
    }
    tree->root->color = COLOR_BLACK;
}

/*
 * Ensures that the red-black conditions continue to hold after a node
 * has been removed from the tree.
 *
 * The node given as a parameter must be the one that replaced the removed node
 * in the removal routine.
 *
 * Algorithm adapted from CLRS.
 */
static void fix_after_removal(libcoll_treemap_t *tree, libcoll_treemap_node_t *replacement)
{
    libcoll_treemap_node_t *node = replacement;
    libcoll_treemap_node_t *sibling;
    while (node != tree->root && COLOR_BLACK == node->color) {
        if (node == node->parent->left) {
            sibling = node->parent->right;
            if (COLOR_RED == sibling->color) {
                // CLRS case 1
                sibling->color = COLOR_BLACK;
                node->parent->color = COLOR_RED;
                left_rotate(tree, node->parent);
                sibling = node->parent->right;
            }
            if (COLOR_BLACK == sibling->left->color  &&
                COLOR_BLACK == sibling->right->color)
            {
                // CLRS case 2
                sibling->color = COLOR_RED;
                node = node->parent;
            } else {
                if (COLOR_BLACK == sibling->right->color) {
                    // CLRS case 3
                    sibling->left->color = COLOR_BLACK;
                    sibling->color = COLOR_RED;
                    right_rotate(tree, sibling);
                    sibling = node->parent->right;
                }
                sibling->color = node->parent->color;
                node->parent->color = COLOR_BLACK;
                sibling->right->color = COLOR_BLACK;
                left_rotate(tree, node->parent);
                node = tree->root;
            }
        } else {
            // symmetric to the if (...) branch
            sibling = node->parent->left;
            if (COLOR_RED == sibling->color) {
                // case 1
                sibling->color = COLOR_BLACK;
                node->parent->color = COLOR_RED;
                right_rotate(tree, node->parent);
                sibling = node->parent->left;
            }
            if (COLOR_BLACK == sibling->right->color  &&
                COLOR_BLACK == sibling->left->color)
            {
                // case 2
                sibling->color = COLOR_RED;
                node = node->parent;
            } else {
                if (COLOR_BLACK == sibling->left->color) {
                    // case 3
                    sibling->right->color = COLOR_BLACK;
                    sibling->color = COLOR_RED;
                    left_rotate(tree, sibling);
                    sibling = node->parent->left;
                }
                sibling->color = node->parent->color;
                node->parent->color = COLOR_BLACK;
                sibling->left->color = COLOR_BLACK;
                right_rotate(tree, node->parent);
                node = tree->root;
            }
        }
    }
    node->color = COLOR_BLACK;
}



/* helper functions for testing purposes */

/*
 * Recursively checks that the red-black tree condition that the children of a
 * red node are black holds in the subtree whose root the given node is.
 *
 * Returns true if the condition holds in the entire subtree, false if not.
 *
 * For internal use in testing.
 */
static bool _verify_child_color_in_subtree(libcoll_treemap_node_t *subtree_root)
{
    bool left_subtree_valid;
    bool right_subtree_valid;
    bool root_valid;
    bool subtree_valid;

    if (NULL_NODE != subtree_root) {
        left_subtree_valid = _verify_child_color_in_subtree(subtree_root->left);
        root_valid = true;
        if (COLOR_RED == subtree_root->color) {
            if (COLOR_RED == subtree_root->left->color  ||
                COLOR_RED == subtree_root->right->color)
            {
                DEBUGF("R-B violation: red node @ %p has a red child\n",
                       (void*) subtree_root);
                root_valid = false;
            }
        }
        right_subtree_valid = _verify_child_color_in_subtree(subtree_root->right);

        subtree_valid = left_subtree_valid && right_subtree_valid && root_valid;
    } else {
        // consider empty subtrees as valid
        subtree_valid = true;
    }
    return subtree_valid;
}

/*
 * Recursively checks that the black-height condition of red-black trees that
 * all paths from a given node to its descendant leaf nodes contain the same
 * number of black nodes holds in a subtree.
 *
 * Returns the black-height of the subtree if the conditions do hold,
 * and -1 if they do not.
 *
 * For internal use in testing.
 */
static int _verify_black_height_of_subtree(libcoll_treemap_node_t *subtree_root)
{
    int subtree_black_height;
    int left_black_height;
    int right_black_height;

    if (NULL_NODE == subtree_root) {
        subtree_black_height = 0;
    } else {
        subtree_black_height = -1;
        left_black_height = _verify_black_height_of_subtree(subtree_root->left);
        right_black_height = _verify_black_height_of_subtree(subtree_root->right);

        if (left_black_height != -1) {
            if (left_black_height == right_black_height) {
                subtree_black_height = left_black_height;
                if (COLOR_BLACK == subtree_root->color) {
                    subtree_black_height++;
                }
            } else {
                DEBUG("R-B violation: ");
                DEBUGF("children of node @ %p have inequal black heights\n",
                       (void*) subtree_root);
                DEBUGF("  Left child at %p: black-height = %d\n",
                       (void*) subtree_root->left, left_black_height);
                DEBUGF("  Right child at %p: black-height = %d\n",
                       (void*) subtree_root->right, right_black_height);
            }
        }
    }
    return subtree_black_height;
}

