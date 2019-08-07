/*
 * hashmap.h
 *
 * a basic implementation of a hashmap
 */

#include <stdlib.h>
#include "linkedlist.h"
#include "node.h"

#ifndef CCOLL_HASHMAP_H
#define CCOLL_HASHMAP_H

typedef struct ccoll_ll_hm_entry {
    void *key;
    void *value;
} ccoll_ll_hm_entry_t;

typedef struct ccoll_ll_hashmap {
    ccoll_linkedlist_t **hash_slots;
    size_t load;
    size_t capacity;
    size_t total_entries;
    double max_load_factor;
    unsigned long (*hash_value_function)(void *key);
    int (*key_comparator_function)(void *key1, void *key2);
    int (*value_comparator_function)(void *value1, void *value2);
} ccoll_ll_hashmap_t;

ccoll_ll_hashmap_t* ccoll_ll_hm_init();

ccoll_ll_hashmap_t* ccoll_ll_hm_init_with_params(size_t init_capacity,
                                 double max_load_factor,
                                 unsigned long (*hash_value_function)(void*),
                                 int (*key_comparator_function)(void *key1, void *key2),
                                 int (*value_comparator_function)(void *value1, void *value2));

void ccoll_ll_hm_deinit(ccoll_ll_hashmap_t *hm);

void ccoll_ll_hm_put(ccoll_ll_hashmap_t *hm, void *key, void *value);

void* ccoll_ll_hm_get(ccoll_ll_hashmap_t *hm, void *key);

char ccoll_ll_hm_contains(ccoll_ll_hashmap_t *hm, void *key);

void* ccoll_ll_hm_remove(ccoll_ll_hashmap_t *hm, void *key);

size_t ccoll_ll_hm_get_capacity(ccoll_ll_hashmap_t *hm);

size_t ccoll_ll_hm_get_size(ccoll_ll_hashmap_t *hm);

#endif  /* CCOLL_HASHMAP_H */