/*
 * hashtable.h
 */

#include <stdlib.h>
#include "linkedlist.h"
#ifndef NODE_H
#include "node.h"
#endif

#ifndef HASHTABLE_H
#define HASHTABLE_H
#endif

#define HASHTABLE_DEFAULT_SIZE          100
#define HASHTABLE_DEFAULT_LOAD_FACTOR   0.75

typedef struct ht_key_value_pair {
    node_value_t *key;
    node_value_t *value;
} ht_key_value_pair_t;

typedef struct hashtable {
    linkedlist_t **hash_slots;
    size_t capacity;
    size_t size;
    unsigned int (*hash_value_function)(node_value_t*);
    int (*compare_function)(node_value_t *v1, node_value_t *v2);
} hashtable_t;


extern hashtable_t* ht_init(size_t init_capacity,
                            unsigned int (*hash_value_function)(node_value_t*),
                            int (*compare_function)(node_value_t *v1, node_value_t *v2));

extern void ht_deinit(hashtable_t *ht);

extern int ht_add(hashtable_t *ht, node_value_t *key, node_value_t *value);

extern node_value_t* ht_get(hashtable_t *ht, node_value_t *key);

extern node_value_t* ht_remove(hashtable_t *ht, node_value_t *key);

extern size_t ht_get_capacity(hashtable_t *ht);

extern size_t ht_get_size(hashtable_t *ht);
