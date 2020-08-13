/*
 * treemap.h
 *
 * An implementation of a generic key-value map as a binary search tree.
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

#include <stdbool.h>
#include <stdlib.h>

#include "types.h"

#ifndef LIBCOLL_TREEMAP_H
#define LIBCOLL_TREEMAP_H

/* data types */

/* A type for representing single nodes in the tree */
typedef struct libcoll_treemap_node {
    struct libcoll_treemap_node *left;
    struct libcoll_treemap_node *right;
    struct libcoll_treemap_node *parent;
    void *key;
    void *value;
    char color;
} libcoll_treemap_node_t;

/* A type for representing the tree itself, for holding useful metadata */
typedef struct libcoll_treemap {
    size_t size;
    libcoll_treemap_node_t *root;
    int (*key_comparator)(const void *key1, const void *key2);
} libcoll_treemap_t;

/* An iterator for iterating through the nodes of a tree in the order of
 * keys as defined by the comparator function (e.g. alphabetical order).
 */
typedef struct libcoll_treemap_iter {
    libcoll_treemap_t *tree;
    libcoll_treemap_node_t *previous;
    libcoll_treemap_node_t *next;
    libcoll_treemap_node_t *last_traversed_node;
} libcoll_treemap_iter_t;


/* external functions */

libcoll_treemap_t* libcoll_treemap_init();

libcoll_treemap_t* libcoll_treemap_init_with_comparator(int (*key_comparator_func)(const void *key1, const void *key2));

void libcoll_treemap_deinit(libcoll_treemap_t *tree);

void libcoll_treemap_deinit_and_delete_contents(libcoll_treemap_t *tree);

libcoll_treemap_node_t* libcoll_treemap_add(libcoll_treemap_t *tree, void *key, void *value);

libcoll_treemap_node_t* libcoll_treemap_get(libcoll_treemap_t *tree, void *key);

bool libcoll_treemap_contains(libcoll_treemap_t *tree, void *key);

libcoll_pair_voidptr_t libcoll_treemap_remove(libcoll_treemap_t *tree, void *key);

libcoll_treemap_node_t* libcoll_treemap_get_successor(libcoll_treemap_node_t *node);

libcoll_treemap_node_t* libcoll_treemap_get_predecessor(libcoll_treemap_node_t *node);

int libcoll_treemap_depth_of(libcoll_treemap_t *tree, void *key);

size_t libcoll_treemap_get_size(libcoll_treemap_t *tree);

char libcoll_treemap_is_empty(libcoll_treemap_t *tree);

libcoll_treemap_iter_t* libcoll_treemap_get_iterator(libcoll_treemap_t *tree);

void libcoll_treemap_drop_iterator(libcoll_treemap_iter_t *iterator);

bool libcoll_treemap_has_next(libcoll_treemap_iter_t *iterator);

libcoll_treemap_node_t* libcoll_treemap_next(libcoll_treemap_iter_t *iterator);

bool libcoll_treemap_has_previous(libcoll_treemap_iter_t *iterator);

libcoll_treemap_node_t* libcoll_treemap_previous(libcoll_treemap_iter_t *iterator);

libcoll_pair_voidptr_t libcoll_treemap_remove_last_traversed(libcoll_treemap_iter_t *iterator);

bool _libcoll_treemap_verify_red_black_conditions(libcoll_treemap_t *tree);



#endif /* LIBCOLL_TREEMAP_H */

