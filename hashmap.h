/*
 * hashmap.h
 *
 * a basic implementation of a hashmap
 */

#include <stdlib.h>
#include "linkedlist.h"
#include "node.h"
#include "types.h"

#ifndef CCOLL_HASHMAP_H
#define CCOLL_HASHMAP_H

typedef struct ccoll_hashmap_entry {
    void *key;
    void *value;
} ccoll_hashmap_entry_t;

typedef struct ccoll_hashmap {
    ccoll_linkedlist_t **hash_slots;
    size_t capacity;
    size_t total_entries;
    double max_load_factor;
    unsigned long (*hash_value_function)(void *key);
    int (*key_comparator_function)(void *key1, void *key2);
    int (*value_comparator_function)(void *value1, void *value2);
} ccoll_hashmap_t;

ccoll_hashmap_t* ccoll_hashmap_init();

ccoll_hashmap_t* ccoll_hashmap_init_with_params(size_t init_capacity,
                                 double max_load_factor,
                                 unsigned long (*hash_value_function)(void*),
                                 int (*key_comparator_function)(void *key1, void *key2),
                                 int (*value_comparator_function)(void *value1, void *value2));

void ccoll_hashmap_deinit(ccoll_hashmap_t *hm);

void ccoll_hashmap_put(ccoll_hashmap_t *hm, void *key, void *value);

void* ccoll_hashmap_get(ccoll_hashmap_t *hm, void *key);

char ccoll_hashmap_contains(ccoll_hashmap_t *hm, void *key);

ccoll_pair_voidptr_t ccoll_hashmap_remove(ccoll_hashmap_t *hm, void *key);

size_t ccoll_hashmap_get_capacity(ccoll_hashmap_t *hm);

size_t ccoll_hashmap_get_size(ccoll_hashmap_t *hm);

char ccoll_hashmap_is_empty(ccoll_hashmap_t *hm);

#endif  /* CCOLL_HASHMAP_H */
