/*
 * node.h
 *
 * Functions and types related to nodes in data structures (e.g. lists).
 */

#include <stdlib.h>

#ifndef CCOLL_NODE_H
#define CCOLL_NODE_H

/*
 * A comparator function for nodes using the identity (i.e. memory address) of
 * the data as a basis of (in)equality.
 */
int _ccoll_node_comparator_memaddr(void *value1, void *value2);

/*
 * A hash value function for nodes using the memory address of the data as the
 * hash value.
 */
unsigned long _ccoll_node_hashvalue_memaddr(void *value);

#endif  /* CCOLL_NODE_H */
