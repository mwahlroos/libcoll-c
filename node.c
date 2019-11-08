/*
 * node.c
 *
 * Functions and types related to nodes in data structures, e.g. lists.
 */

#include "node.h"

int _libcoll_node_comparator_memaddr(void *node1, void *node2)
{
    int cmpval;
    if (node1 == node2) {
        cmpval = 0;
    } else if (node1 < node2) {
        cmpval = -1;
    } else {
        cmpval = 1;
    }
    return cmpval;
}

unsigned long _libcoll_node_hashvalue_memaddr(void *value)
{
    return (unsigned long) value;
}
