/*
 * hashmap.h
 *
 * a basic implementation of a hashmap
 */

#include <stdlib.h>
#include "linkedlist.h"
#include "node.h"

#ifndef HASHMAP_H
#define HASHMAP_H

typedef struct hm_entry {
    void *key;
    void *value;
} hm_entry_t;

typedef struct hashmap {
    linkedlist_t **hash_slots;
    size_t load;
    size_t capacity;
    size_t total_entries;
    double max_load_factor;
    unsigned long (*hash_value_function)(void *key);
    int (*key_comparator_function)(void *key1, void *key2);
    int (*value_comparator_function)(void *value1, void *value2);
} hashmap_t;

hashmap_t* hm_init();

hashmap_t* hm_init_with_params(size_t init_capacity,
                                 double max_load_factor,
                                 unsigned long (*hash_value_function)(void*),
                                 int (*key_comparator_function)(void *key1, void *key2),
                                 int (*value_comparator_function)(void *value1, void *value2));

void hm_deinit(hashmap_t *hm);

void hm_put(hashmap_t *hm, void *key, void *value);

void* hm_get(hashmap_t *hm, void *key);

char hm_contains(hashmap_t *hm, void *key);

void* hm_remove(hashmap_t *hm, void *key);

size_t hm_get_capacity(hashmap_t *hm);

size_t hm_get_size(hashmap_t *hm);

#endif  /* HASHMAP_H */
