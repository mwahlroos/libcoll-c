/*
 * hashtable.h
 *
 * a basic implementation of a hashtable (or rather a hashmap)
 */

#include <stdlib.h>
#include "linkedlist.h"
#include "node.h"

#ifndef HASHTABLE_H
#define HASHTABLE_H

typedef struct ht_key_value_pair {
    void *key;
    void *value;
} ht_key_value_pair_t;

typedef struct hashtable {
    linkedlist_t **hash_slots;
    size_t capacity;
    size_t size;
    unsigned int (*hash_value_function)(void *key);
    int (*key_comparator_function)(void *key1, void *key2);
    int (*value_comparator_function)(void *value1, void *value2);
} hashtable_t;

extern hashtable_t* ht_init();

extern hashtable_t* ht_init_with_params(size_t init_capacity,
                                        unsigned int (*hash_value_function)(void*),
                                        int (*key_comparator_function)(void *key1, void *key2),
                                        int (*value_comparator_function)(void *value1, void *value2));

extern void ht_deinit(hashtable_t *ht);

extern int ht_add(hashtable_t *ht, void *key, void *value);

extern void* ht_get(hashtable_t *ht, void *key);

extern void* ht_remove(hashtable_t *ht, void *key);

extern size_t ht_get_capacity(hashtable_t *ht);

extern size_t ht_get_size(hashtable_t *ht);

#endif  /* HASHTABLE_H */
