/*
 * node.h
 *
 * Functions and types related to nodes in data structures (e.g. lists).
 */

#include <stdlib.h>

#ifndef NODE_H
#define NODE_H
#endif

typedef struct {
    size_t size;
    void *payload;
} node_value_t;

extern int _node_memaddr_comparator(node_value_t *v1, node_value_t *v2);

