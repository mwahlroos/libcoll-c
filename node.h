/*
 * node.h
 *
 * Functions and types related to nodes in data structures (e.g. lists).
 */

#include <stdlib.h>

#ifndef LIBCOLL_NODE_H
#define LIBCOLL_NODE_H

/*
 * A comparator function for nodes using the identity (i.e. memory address) of
 * the data as a basis of (in)equality.
 */
int _libcoll_node_comparator_memaddr(void *value1, void *value2);

/*
 * A hash value function for nodes using the memory address of the data as the
 * hash value.
 */
unsigned long _libcoll_node_hashvalue_memaddr(void *value);

#endif  /* LIBCOLL_NODE_H */
