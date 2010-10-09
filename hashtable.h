/*
 * hashtable.h
 */

#include <stdlib.h>
#include "node.h"

#ifndef HASHTABLE_H
#define HASHTABLE_H
#endif

#define HASHTABLE_DEFAULT_SIZE  100

typedef struct ht_key {
    size_t size;
    void *key;
} ht_key_t;



typedef struct hashtable {
    ht_key_t *start;
    size_t capacity;
    unsigned int (*hash_value_function)(ht_key_t*);
} hashtable_t;


extern hashtable_t* ht_init(size_t init_capacity,
                            unsigned int (*hash_value_function)(ht_key_t*));

extern void ht_deinit(hashtable_t *ht);

extern int ht_add(hashtable_t *ht, ht_key_t *key, node_value_t *value);

extern node_value_t* ht_get(hashtable_t *ht, ht_key_t *key);

extern node_value_t* ht_remove(hashtable_t *ht, ht_key_t *key);

extern size_t ht_get_capacity(hashtable_t *ht);
