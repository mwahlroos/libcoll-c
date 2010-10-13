/*
 * node.h
 *
 * Functions and types related to nodes in data structures (e.g. lists).
 */

#include <stdlib.h>

#ifndef NODE_H
#define NODE_H

typedef struct {
    size_t size;
    void *payload;
} node_value_t;

/*
 * A comparator function for nodes using the identity (i.e. memory address) of
 * the data as a basis of (in)equality.
 */
extern int _node_comparator_memaddr(node_value_t *v1, node_value_t *v2);

/*
 * A hash value function for nodes using the memory address of the data as the
 * hash value.
 */
extern unsigned int _node_hashvalue_memaddr(node_value_t *value);

#endif
