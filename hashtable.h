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
    size_t load;
    size_t capacity;
    size_t total_entries;
    double max_load_factor;
    unsigned long (*hash_value_function)(void *key);
    int (*key_comparator_function)(void *key1, void *key2);
    int (*value_comparator_function)(void *value1, void *value2);
} hashtable_t;

hashtable_t* ht_init();

hashtable_t* ht_init_with_params(size_t init_capacity,
                                 double max_load_factor,
                                 unsigned long (*hash_value_function)(void*),
                                 int (*key_comparator_function)(void *key1, void *key2),
                                 int (*value_comparator_function)(void *value1, void *value2));

void ht_deinit(hashtable_t *ht);

void ht_put(hashtable_t *ht, void *key, void *value);

void* ht_get(hashtable_t *ht, void *key);

char ht_contains(hashtable_t *ht, void *key);

void* ht_remove(hashtable_t *ht, void *key);

size_t ht_get_capacity(hashtable_t *ht);

size_t ht_get_size(hashtable_t *ht);

#endif  /* HASHTABLE_H */
